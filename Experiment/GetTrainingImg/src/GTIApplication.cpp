#include "GTIApplication.h"

GTIApplication* GTIApplication::singleton = NULL;

GTIApplication::GTIApplication():
	videoLen(0),
	playSpeed(0),
	sec(0),
	fid(0),
	bar_file_idx(0),
	other_file_idx(0)
{		
}

GTIApplication::~GTIApplication()
{
}

GTIApplication* GTIApplication::getSingleton()
{
	if(!singleton)
		singleton = new GTIApplication();
	
	return singleton;
}

void GTIApplication::go()
{	
	//_mkdir("sample");
	_mkdir(Pubvar::trainingImgPath.c_str());
	_mkdir((Pubvar::trainingImgPath + "/hump").c_str());
	_mkdir((Pubvar::trainingImgPath + "/hump/mask").c_str());
	_mkdir((Pubvar::trainingImgPath + "/anomaly").c_str());
	_mkdir((Pubvar::trainingImgPath + "/anomaly/mask").c_str());
	
	CV_Assert(capture.open(Pubvar::videoPath));	
	videoLen = (int)capture.get(CV_CAP_PROP_FRAME_COUNT);		
	fps = (int)capture.get(CV_CAP_PROP_FPS);	

	int height = (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT),
		width = (int)capture.get(CV_CAP_PROP_FRAME_WIDTH);	
	//object detection ROI
	rectROI = Rect(Pubvar::ROI_LEFT * width, Pubvar::ROI_UP * height, 
					(Pubvar::ROI_RIGHT - Pubvar::ROI_LEFT) * width, (Pubvar::ROI_DOWN - Pubvar::ROI_UP) * height);	//up-left, right-down

	for (fid=0; fid < videoLen; ++fid)
	{
		sec = fid/fps;
		capture >> frame;			
		processing();
		imshow(Pubvar::videoPath, frame);	
		imshow("obj", obj);
		imshow("objMask", objMask);
		imshow("ROI",frame(rectROI));

		createTrackbar( "time", Pubvar::videoPath, &sec, videoLen/fps ,fidControler, (void*)this);		
		if(!keyboardCtrl( waitKey(playSpeed) ))
			break;
	}			
}

void GTIApplication::fidControler(int val, void* _this)
{	
	GTIApplication* __this = (GTIApplication*)_this;
	__this->fid = __this->sec * __this->fps;
	__this->capture.set(CV_CAP_PROP_POS_FRAMES, __this->fid );	
}

void GTIApplication::processing()
{			
	cvtColor(frame(rectROI),ROI,CV_BGR2GRAY);
	ObjDetection::getSingleton()->objDetection(ROI, objMask);
	obj = Scalar::all(0);
	frame(rectROI).copyTo(obj, objMask);	
	
	Mat descriptor, mCornor;
	vector<KeyPoint> cornor;
	SURF surf;
	surf(frame(rectROI), objMask, cornor, descriptor);			
	
	drawKeypoints(obj, cornor,  mCornor, Scalar::all(-1),DrawMatchesFlags::DEFAULT);  
	imshow("mCornor", mCornor);
}

bool GTIApplication::keyboardCtrl(char btn)
{
	switch(btn)
	{
		case 27:	//ESC
			return false;
		case 'p':
		    playSpeed ^= 1;
		    break;
		case 'a':	//a: backward
			fid = fid-2*fps > 0 ? fid-2*fps : 0;
		    capture.set(CV_CAP_PROP_POS_FRAMES,fid);
		    break;
		case 'd':	//d: forward
		    fid = fid+2*fps < videoLen ? fid+2*fps : videoLen;
		    capture.set(CV_CAP_PROP_POS_FRAMES,fid);
		    break;
		case 'b':
			saveImage(HUMP);
			break;
		case 'v':
			saveImage(ANOMALY);
			break;
	}
	return true;
}

void GTIApplication::saveImage(int opt)
{	
	char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
	_splitpath_s( Pubvar::videoPath.c_str(), drive, dir, fname, ext);
	string strFname = fname;
	strFname +="_";
	if(opt==HUMP)
	{		
		imwrite(Pubvar::trainingImgPath + "/hump/" + strFname + to_string(bar_file_idx)+".jpg" ,  frame(rectROI));		
		imwrite(Pubvar::trainingImgPath + "/hump/mask/" + strFname + to_string(bar_file_idx++)+".jpg" ,  objMask);
	}
	else if(opt==ANOMALY)
	{		
		imwrite(Pubvar::trainingImgPath + "/anomaly/" + strFname + to_string(other_file_idx)+".jpg" ,  frame(rectROI));
		imwrite(Pubvar::trainingImgPath + "/anomaly/mask/" + strFname + to_string(other_file_idx++)+".jpg" ,  objMask);
	}
}