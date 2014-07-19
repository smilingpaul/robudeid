#include "ORapplication.h"

ORapplication::ORapplication():
	fid(0),
	sec(0),
	playSpeed(0)	
{			
	recog = new ObjRecog(Pubvar::SVMmodel.c_str(), Pubvar::dictPath.c_str());
}

ORapplication::~ORapplication()
{
	capture.release();
}

ORapplication* ORapplication::singleton = NULL;
ORapplication* ORapplication::getSingleton()
{
	if(!singleton)
		singleton = new ORapplication();
	
	return singleton;
}

void ORapplication::go()
{	
	CV_Assert(capture.open(Pubvar::videoPath));	
	videoLen = (int)capture.get(CV_CAP_PROP_FRAME_COUNT);	
	fps = (int)capture.get(CV_CAP_PROP_FPS);					

	int height = (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT),
		width = (int)capture.get(CV_CAP_PROP_FRAME_WIDTH);	
	//object detection ROI
	rectROI = Rect(Pubvar::ROI_LEFT * width, Pubvar::ROI_UP * height, 
					(Pubvar::ROI_RIGHT - Pubvar::ROI_LEFT) * width, (Pubvar::ROI_DOWN - Pubvar::ROI_UP) * height);	//up-left, right-down
		
	cout<<"[a]: rewind"<<endl<<"[d]: forward"<<endl<<"[p]: pause / continue"<<endl<<"[o]: object detection"<<endl;

	for (fid=0; fid < videoLen; ++fid)
	{
		sec = fid/fps;
		capture >> frame;		
		processing();		

		imshow(Pubvar::videoPath, frame);			
		createTrackbar( "time", Pubvar::videoPath, &sec, videoLen/fps ,fidControler, (void*)this);				
		
		if(!keyboardCtrl( waitKey(playSpeed) ))
			break;
	}
}

inline void ORapplication::processing()
{	
	Mat roadMask, road, objMask, obj;
	cvtColor(frame(rectROI),ROI,CV_BGR2GRAY);
	
	//ObjDetection::getSingleton()->roadSegmentation(ROI,roadMask);
	//ObjDetection::getSingleton()->fill_hole(roadMask,roadMask);
	//imshow("road", roadMask);

	ObjDetection::getSingleton()->objDetection(ROI, objMask);
	obj = frame(rectROI).clone();
	add(obj, Scalar(0,0,255), obj, objMask);
	imshow("ROI", ROI);
	imshow("detected obj", obj);
	pair<double,double> label_pr = recog->objEstimate(frame(rectROI), objMask);

	double pr = 100 * (label_pr.first == HUMP? label_pr.second : 1-label_pr.second);		//probability of raod hump
	putText(frame, "HUMP: " + to_string(pr) + "%" , Point(0,frame.rows/2), cv::FONT_HERSHEY_SIMPLEX, 2.5, CV_RGB(255,0,0),3 );	
	//pair<double,double> label_pr = ObjRecog::getSingleton()->objEstimate(frame(rectROI), objMask);
	cout<< label_pr.first<<"   "<< label_pr.second<<endl;	
}

void ORapplication::fidControler(int val, void* _this)
{	
	ORapplication* __this = (ORapplication*)_this;
	__this->fid = __this->sec * __this->fps;		
	__this->videoMove(__this->fid);	
	__this->capture >> __this->frame;	//preview
	imshow(Pubvar::videoPath, __this->frame);	
	++__this->fid;		//process next frame
}

inline void ORapplication::videoMove(int _fid)
{
	capture.set(CV_CAP_PROP_POS_FRAMES,_fid);
}

bool ORapplication::keyboardCtrl(char btn)
{
	switch(btn)
	{
		case 27:	//ESC
			return false;

		case 'p':
		    playSpeed ^= 1;
		    break;

		case 'a':	//a: rewind
			fid = fid-2*fps > 0 ? fid-2*fps : 0;
			videoMove(fid);		    
		    break;

		case 'd':	//d: forward
		    fid = fid+2*fps < videoLen ? fid+2*fps : videoLen;
		    videoMove(fid);
		    break;
	}
	return true;
}