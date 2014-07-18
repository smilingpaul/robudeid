#include "ObjDetection.h"

ObjDetection::ObjDetection():
	sobelDoubleThr(1, 256, CV_8U),
	minThr(40),
	diOfError(2.5),		//error = diOfError * di + mdOfError  * md
	mdOfError(5)
{	
	for( int i = 0; i < 256; ++i)	//double thresholding after sobel filter (object detection)
	{
		if(i>80)
			sobelDoubleThr.data[i] = 255;
		else if(i>20)
			sobelDoubleThr.data[i] = POWTRANSFORM(i-20,60,0.5);
		else
			sobelDoubleThr.data[i] = 0;
	}
	a=5;	//obj linear combinaiton's coefficient
	b=10;
	c=10;
}

ObjDetection::~ObjDetection()
{
}

ObjDetection* ObjDetection::singleton = NULL;
ObjDetection* ObjDetection::getSingleton()
{
	if(!singleton)
		singleton = new ObjDetection();
	
	return singleton;
}

//refine ROI
void ObjDetection::roadSegmentation(Mat src, Mat& dst)
{		
	dst.create(src.size(), src.type());					
	
	Mat lutRoad(1, 256, CV_8U);
	//medianBlur(src, meta, 5);

	//init
	if(mixtureModule.empty())
		mixtureModule = src.clone();
	if(energy.empty())	
	{				
		//energy = Scalar::all(255);
		energy = Mat::ones(src.size(), src.type()) * 64;	//energy0 = 64
		circle(energy, Point(energy.cols/2, energy.rows-1), energy.cols/4, Scalar(0),-1);
	}	
	if(road.empty())	
	{		
		road = Mat::zeros(src.size(), src.type());
		circle(road, Point(road.cols/2, road.rows-1), road.cols/4, Scalar(255),-1);		
	}
	
	dIntensity = triangleThr(src, road);	// get dominant intensity threshold
	for( int i = 0; i < 256; ++i)	// look-up table by threshold
	{		
		if(i<dIntensity.first)
			lutRoad.data[i] = POWTRANSFORM((dIntensity.first-i),dIntensity.first,0.5);
		else if(i>dIntensity.second)
			lutRoad.data[i] = POWTRANSFORM((i-dIntensity.second),255-dIntensity.second,0.5);
		else
			lutRoad.data[i] = 0;
	}

	LUT(src, lutRoad, diDiff);	//compute dominant intensity difference
	absdiff(src, mixtureModule, moduleDiff);	//compute module difference
	addWeighted(diDiff, diOfError, moduleDiff, mdOfError, 0, error);	//compute error by combining dominant intensity difference and module difference
	//imshow("dominant intensity Diff",diDiff);
	//imshow("model Diff",moduleDiff);
	//imshow("error",error);
		
	Mat cmpImg, cmpNE0;
	//update energy
	compare(error, Scalar(0), cmpNE0, CMP_NE);
	compare(error, energy, cmpImg, CMP_GT);	
	cmpImg &= cmpNE0;	//update when error occur
	add(energy, Scalar(1), energy, cmpImg);		
	compare(error, energy, cmpImg, CMP_LT);	
	cmpImg &= cmpNE0;
	subtract(energy, Scalar(1), energy, cmpImg);	
	//update mixture model
	compare(src, mixtureModule, cmpImg, CMP_GT);
	add(mixtureModule, Scalar(1),mixtureModule, cmpImg);
	compare(src, mixtureModule, cmpImg, CMP_LT);		
	subtract(mixtureModule, Scalar(1), mixtureModule, cmpImg);
	//energy processing	
	threshold(energy, road, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
	Mat element = getStructuringElement(MORPH_RECT, Size(11,11));
	erode( road, road, element);	
	principalConnectedComponent(road, road);
	dilate(road, road, element);			
	//imshow("mixtureModule", mixtureModule);
	//imshow("energy", energy);	
	//imshow("road", road);
	road.copyTo(dst);
}

void ObjDetection::aControler(int val, void* _this)
{	
	ObjDetection* __this = (ObjDetection*)_this;
	__this->a = val;
	__this->calObj();
}

void ObjDetection::bControler(int val, void* _this)
{	
	ObjDetection* __this = (ObjDetection*)_this;
	__this->b = val;
	__this->calObj();
}

void ObjDetection::cControler(int val, void* _this)
{	
	ObjDetection* __this = (ObjDetection*)_this;
	__this->c = val;
	__this->calObj();
}

void ObjDetection::calObj()
{
	addWeighted(diDiff, (double)a/10, moduleDiff, (double)b/10, 0, objProbality);
	addWeighted(objProbality, 1, gxy, (double)c/10, 0, objProbality);
		
	imshow("object density function (ODF)", objProbality);
}

void ObjDetection::objDetection(Mat frame, Mat& obj)	
{		
	Mat roadMask;
	this->roadSegmentation(frame,roadMask);		
	fill_hole(roadMask,roadMask);
	
	Mat gx, gy;
	//medianBlur(frame, obj, 5);
	Sobel( frame, gx, CV_16S, 1, 0, 3 );	// Gradient X
	Sobel( frame, gy, CV_16S, 0, 1, 3 );	// Gradient Y
	convertScaleAbs( gx, gx );
	convertScaleAbs( gy, gy );
	addWeighted( gx, 0.5, gy, 0.5, 0, gxy );	
	
	//imshow("sobel", gxy);	
	LUT(gxy, sobelDoubleThr, gxy);
	GaussianBlur( gxy, gxy, Size(5,5), 0, 0, BORDER_DEFAULT );
	imshow("Edge Neighbor", gxy);
	calObj();
	createTrackbar( "Intensity Diff", "object density function (ODF)", &a, 10 ,aControler, (void*)this);	
	createTrackbar( "Model Diff", "object density function (ODF)", &b, 10 ,bControler, (void*)this);	
	createTrackbar( "Edge Neighbor", "object density function (ODF)", &c, 10 ,cControler, (void*)this);	

	Mat roadTmp;
	Mat mergeKer = getStructuringElement( MORPH_RECT, Size(5, 5)),
		denoiseKer = getStructuringElement( MORPH_RECT, Size(3, 3)),
		reduceRoad = getStructuringElement(MORPH_RECT, Size(17,17));	

	erode( roadMask, roadTmp, reduceRoad);	
	imshow("road mask", roadTmp);
	objProbality = objProbality & roadTmp;
	if(threshold(objProbality, obj, 50, 255, THRESH_BINARY | THRESH_OTSU) < minThr)
	{
		obj = Mat::zeros(frame.size(), frame.type());
		return;
	}

	morphologyEx(obj, obj, MORPH_CLOSE, mergeKer);
	fill_hole(obj,obj);			
	morphologyEx(obj, obj, MORPH_OPEN , denoiseKer);
}


pair<int,int> ObjDetection::triangleThr(Mat src, Mat mask)
{			
	int histSize = 256;	// Establish the number of bins
	float range[] = { 0, 256 } ;	// Set the ranges
    const float* histRange = { range };	
	Mat hist;
	calcHist( &src, 1, 0, mask, hist, 1, &histSize, &histRange, true, false );	
	medianBlur(hist,hist,5);	//prevent peak effect

	Point maxLoc_p; 
	double maxVal = 0	;
	minMaxLoc(hist, NULL, &maxVal, NULL, &maxLoc_p);
	
	int maxLoc = maxLoc_p.y;	
	int lowthr = 0, highthr = maxLoc, dist = 0;

	for(int i=0; i<maxLoc; ++i)
	{
		//(0,0) + (maxLoc, maxVal) => maxval*x - maxLoc*y = 0
		int _dist = maxVal * i - maxLoc * hist.at<float>(i);		
		if(dist < _dist)
		{			
			dist = _dist;
			lowthr = i;
		}		
	}	
		
	dist = 0;
	for(int i=maxLoc+1; i<256; ++i)
	{
		//(maxLoc, maxVal) (255,0)=> maxVal*x - (maxLoc-255)*y - 255*maxVal = 0
		int _dist = maxVal * i - (maxLoc-255) * hist.at<float>(i) - 255*maxVal;			
		if(dist > _dist)
		{			
			dist = _dist;
			highthr = i;
		}		
	}

	return make_pair(lowthr,highthr);
}

void ObjDetection::fill_hole(Mat src, Mat& dst)
{
	dst.create(src.size(), src.type());	

	Mat mask = src.clone();
	for (int i = 0; i < mask.cols; i++) 
	{
		if (mask.at<char>(0, i) == 0) 
			floodFill(mask, Point(i, 0), 255);
		
		if (mask.at<char>(mask.rows-1, i) == 0) 
			floodFill(mask, Point(i, mask.rows-1), 255);		
	}

	for (int i = 0; i < mask.rows; i++) 
	{
		if (mask.at<char>(i, 0) == 0) 
			floodFill(mask, Point(0, i), 255);
    
		if (mask.at<char>(i, mask.cols-1) == 0) 
			floodFill(mask, Point(mask.cols-1, i), 255);
    
	}

	bitwise_xor(src, mask, dst);
	bitwise_not(dst, dst);
	//imshow("filled image", mask);
}

void ObjDetection::principalConnectedComponent(Mat src, Mat& dst)
{	
	dst.create(src.size(), src.type());	
		
	Mat blob = src.clone();	
	Point p; //component point
	int maxArea;
	for(int y=0; y < blob.rows; y++) 
	{     
		//uchar *row = (uchar*)blob.ptr(y);
        for(int x=0; x < blob.cols; x++) 
		{			
            if(blob.data[blob.cols*y + x] == 0) //black			
                continue;         
			
            Rect rect;					
            floodFill(blob, Point(x,y), 0, &rect);
			if(rect.area()>maxArea && (rect.y+rect.height/2) > src.rows/2)
			{
				//cout<<rect.y<<" "<<rect.height/2<<" "<<src.rows/2<<endl;
				maxArea = rect.area();
				p = Point(x,y);
			}  
        }
    }	
	Mat __src = src.clone();
	floodFill(__src, p, 0);
	absdiff(src, __src, dst);	
}

// show histogram 	
void ObjDetection::showHist(Mat src)
{
	// Establish the number of bins
	int histSize = 256;
	// Set the ranges
	float range[] = { 0, 256 } ;
    const float* histRange = { range };
	bool uniform = true; bool accumulate = false;

	Mat hist;
	calcHist( &src, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );
	
	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound( (double) hist_w/histSize );

	Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );	

	for( int i = 1; i < histSize; i++ )
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
					   CV_RGB(255, 0, 0), 2, 8, 0  );
	imshow("histogram", histImage );
}