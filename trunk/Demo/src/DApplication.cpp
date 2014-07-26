#include "DApplication.h"

DApplication::DApplication():
	fid(0),
	sec(0),
	playSpeed(0),
	hLineGraph(new Graph()),
	vLineGraph(new Graph())	,
	uneven(false)
{	
	if(Pubvar::sensorMethod==BLOCK_MATCHING)
		sensor = new BlockMatchingSensor();
	else if(Pubvar::sensorMethod==PHASE_BASED)
		sensor = new PhaseBasedSensor();	
	else if(Pubvar::sensorMethod==G_SENSOR)
		sensor = new GSensor(Pubvar::nmeaPath);	

	recog = new ObjRecog(Pubvar::SVMmodel.c_str(), Pubvar::dictPath.c_str());	
}

DApplication::~DApplication()
{
}

DApplication* DApplication::singleton = NULL;
DApplication* DApplication::getSingleton()
{
	if(!singleton)
		singleton = new DApplication();
	
	return singleton;
}

void DApplication::go()
{		
	CV_Assert(capture.open(Pubvar::videoPath));	
	videoLen = (int)capture.get(CV_CAP_PROP_FRAME_COUNT);	
	fps = (int)capture.get(CV_CAP_PROP_FPS);			
	bDtr = new BDetector(fps, Pubvar::sensitive);		
	objType = new int[videoLen];
	for(int i=0; i<videoLen; ++i)
		objType[i] = ANOMALY;

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
		isSkip = false;
		imshow(Pubvar::videoPath, frame);			
		createTrackbar( "time", Pubvar::videoPath, &sec, videoLen/fps ,fidControler, (void*)this);				
		
#ifdef RECORD_VIDEO
		this->videoRecorder(frame);
#endif // RECORD_VIDEO

		if(!keyboardCtrl( waitKey(playSpeed) ))
			break;				
	}

#ifdef RECORD_VIDEO
	demoWriter.release();
#endif // RECORD_VIDEO
	capture.release();
}

inline void DApplication::processing()
{	
	Mat roadMask, road, objMask, obj;
	Point2d shift = sensor->sense(frame,fid);
	if(isSkip)	//video move forward or backward, therefore shift will become 0 now
		shift = Point2d(0,0);

	hLineGraph->insert( (int)shift.x );
	vLineGraph->insert( (int)shift.y );

	imshow("horizontal", hLineGraph->update().t());
	imshow("vertical", vLineGraph->update());

	bDtr->update(shift);
	if(bDtr->isBump()!= NOBUMP)
	{
		bool humpFlag = false;
		for(int i=fid - 4*fps; i<=fid; ++i)
		{
			if(objType[i]==HUMP)
			{
				humpFlag = true;
				break;
			}
		}

		if(humpFlag)
			putText(frame, "Hump uneven!! " , Point(0,frame.rows/2), cv::FONT_HERSHEY_SIMPLEX, 2.5, CV_RGB(255,0,0),3 );	
		else
			putText(frame, "Invisible uneven!! " , Point(0,frame.rows/2), cv::FONT_HERSHEY_SIMPLEX, 2.5, CV_RGB(255,0,0),3 );	
	}
		

	cvtColor(frame(rectROI),ROI,CV_BGR2GRAY);
	ObjDetection::getSingleton()->objDetection(ROI, objMask);
	obj = frame(rectROI).clone();
	add(obj, Scalar(0,0,255), obj, objMask);
	imshow("ROI", ROI);
	imshow("detected obj", obj);
	pair<double,double> label_pr = recog->objEstimate(frame(rectROI), objMask);

	double pr = 100 * (label_pr.first == HUMP? label_pr.second : 1-label_pr.second);		//probability of raod hump
	cout<< pr <<endl;
	//putText(frame, "HUMP: " + to_string(pr) + "%" , Point(frame.cols/4, frame.rows/4), cv::FONT_HERSHEY_SIMPLEX, 1.5, CV_RGB(255,0,0),3 );	

	double pr_bar = bDtr->updateBumpEvt(label_pr);
	putText(frame, to_string(int(pr_bar*100)) + "%" , Point(frame.cols/2, frame.rows/4), 
					cv::FONT_HERSHEY_SIMPLEX, 1.5, CV_RGB(255,0,0),3 );
	
	if(bDtr->barExist())
		objType[fid] = HUMP;
	else
		objType[fid] = ANOMALY;

}

void DApplication::fidControler(int val, void* _this)
{	
	DApplication* __this = (DApplication*)_this;
	__this->fid = __this->sec * __this->fps;		
	__this->videoMove(__this->fid);	
	__this->capture >> __this->frame;	//preview
	imshow(Pubvar::videoPath, __this->frame);	
	++__this->fid;		//process next frame
}

inline void DApplication::videoMove(int _fid)
{
	isSkip = true;	
	capture.set(CV_CAP_PROP_POS_FRAMES,_fid);
	sensor->reset();
	hLineGraph->clear();
	vLineGraph->clear();
	bDtr->evtClear();
}

inline bool DApplication::keyboardCtrl(char btn)
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

inline void DApplication::videoRecorder(Mat& img)
{
	if(!demoWriter.isOpened())
		demoWriter = VideoWriter(Pubvar::demoVideoPath, CV_FOURCC('D', 'I', 'V', '3') , fps, img.size());
	
	demoWriter << img;
}