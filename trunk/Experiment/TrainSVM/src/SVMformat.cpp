#include "SVMformat.h"

int main( int argc, char** argv )
{
	SVMformat::getSingleton()->go();
}

SVMformat* SVMformat::singleton = NULL;

SVMformat::SVMformat():
	imgPath("sample/training_img/"),
	bowVocSize(512)
{	
	CV_Assert(readBowCodebook());
}


SVMformat::~SVMformat()
{
}

SVMformat* SVMformat::getSingleton()
{
	if(!singleton)
		singleton = new SVMformat();
	
	return singleton;
}

void SVMformat::go()
{		
	this->readImgs(imgPath + "/hump/");
	writeLibSVMformat(HUMP);	
	this->readImgs(imgPath + "/anomaly/");	
	writeLibSVMformat(ANOMALY);
}

void SVMformat::readImgs(string path)
{		
	struct _finddata_t c_file;
	long hFile;	
	string filter = path + "*.jpg";		
	hFile = _findfirst(filter.c_str(), &c_file);	
	CV_Assert(hFile != -1);

	Mat img, objMask;
	do
	{		
		img = imread(path + c_file.name);			
		objMask = imread( path + "mask/" + c_file.name, IMREAD_GRAYSCALE);					
		bowExtractor(img, objMask);			
	}
	while(_findnext( hFile, &c_file )==0 );		
}


void SVMformat::bowExtractor(Mat img, Mat mask)
{
	Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);		//KNN
	vector<KeyPoint> cornor; 
	int minHessian = 400; //SURF Hessian Threshold
	Ptr<FeatureDetector> detector(new SurfFeatureDetector());
	Ptr<DescriptorExtractor> extractor(new SurfDescriptorExtractor(minHessian,4,2,false));
	
	BOWImgDescriptorExtractor bowDE(extractor,matcher);
	bowDE.setVocabulary(dictionary);
	Mat bowDescriptor;	
	
	detector->detect(img, cornor, mask);		
	if(cornor.size() < 10)
		return;

	bowDE.compute(img, cornor, bowDescriptor);	
	bowDescriptor = bowDescriptor*100;
	bowDescSet.push_back(bowDescriptor);	//have to convert to CV_8U before training	
}

void SVMformat::writeLibSVMformat(int label)
{
	CV_Assert(!bowDescSet.empty());
	bowDescSet.convertTo(bowDescSet, CV_8U);
	fstream fp;
	fp.open("sample/libSVMTrainingData.txt", ios::app);
	for(int i=0; i<bowDescSet.rows; ++i)
	{
		fp << label << " ";
		for(int j=0; j<bowDescSet.cols; ++j)
			fp << j+1 << ":" << (int)bowDescSet.at<uchar>(i,j)<<" ";
		fp<<endl;
	}
	fp.close();
	bowDescSet= Mat();
}

bool SVMformat::readBowCodebook()
{	 
	string dictPath = "sample/dictionary-" + to_string(bowVocSize) + ".yml";
    FileStorage fs(dictPath, FileStorage::READ);
	if( fs.isOpened() )
    {
         fs["dictionary"] >> dictionary;
        return true;
    }
    return false;    
}