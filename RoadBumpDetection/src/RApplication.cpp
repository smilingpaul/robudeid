#include "Rapplication.h"

RApplication::RApplication():
	fid(0),
	sec(0),
	playSpeed(0),
	hLineGraph(new Graph()),
	vLineGraph(new Graph())	,
	uneven(false)
	//lastBumpfid(0)
{	
	if(Pubvar::sensorMethod==BLOCK_MATCHING)
		sensor = new BlockMatchingSensor();
	else if(Pubvar::sensorMethod==PHASE_BASED)
		sensor = new PhaseBasedSensor();	
	else if(Pubvar::sensorMethod==G_SENSOR)
		sensor = new GSensor(Pubvar::nmeaPath);	
}

RApplication::~RApplication()
{
	capture.release();
}

RApplication* RApplication::singleton = NULL;
RApplication* RApplication::getSingleton()
{
	if(!singleton)
		singleton = new RApplication();
	
	return singleton;
}

void RApplication::go()
{	
	CV_Assert(capture.open(Pubvar::videoPath));	
	videoLen = (int)capture.get(CV_CAP_PROP_FRAME_COUNT);	
	fps = (int)capture.get(CV_CAP_PROP_FPS);			
	bDtr = new BDetector(fps, Pubvar::sensitive);	
	fidOI = new bool[videoLen];
	for(int i=0; i<videoLen;++i)
		fidOI[i] = false;

	cout<<"[a]: rewind"<<endl<<"[d]: forward"<<endl<<"[p]: pause / continue"<<endl<<"[o]: object detection"<<endl;

	for (fid=0; fid < videoLen; ++fid)
	{
		sec = fid/fps;
		capture >> frame;		
		processing();
		isSkip = false;

#ifdef DEMO
		imshow(Pubvar::videoPath, frame);			
		createTrackbar( "time", Pubvar::videoPath, &sec, videoLen/fps ,fidControler, (void*)this);				
		
		if(!keyboardCtrl( waitKey(playSpeed) ))
			break;

#elif defined EXPERIMENT			
		if(fid %(videoLen/10) ==0)
		{
			system("cls");
			cout<<Pubvar::videoPath <<" :"<< ((double)fid/videoLen) * 100<<" %"<<endl;			
		}				
#endif
	}

#ifdef EXPERIMENT
	this->writeVideo();	
#endif
}

inline void RApplication::processing()
{		
	Point2d shift = sensor->sense(frame,fid);
	if(isSkip)	//video move forward or backward, therefore shift will become 0 now
		shift = Point2d(0,0);

	hLineGraph->insert( (int)shift.x );
	vLineGraph->insert( (int)shift.y );
	//cout<<shift<<" "<<fid<<endl;
#ifdef DEMO
	imshow("horizontal", hLineGraph->update().t());
	imshow("vertical", vLineGraph->update());
#endif

	
	bDtr->update(shift);
	if(bDtr->isBump()!= NOBUMP)
		putText(frame, "uneven!! " , Point(0,frame.rows/2), cv::FONT_HERSHEY_SIMPLEX, 2.5, CV_RGB(255,0,0),3 );	
}

void RApplication::fidControler(int val, void* _this)
{	
	RApplication* __this = (RApplication*)_this;
	__this->fid = __this->sec * __this->fps;		
	__this->videoMove(__this->fid);	
	__this->capture >> __this->frame;	//preview
	imshow(Pubvar::videoPath, __this->frame);	
	++__this->fid;		//process next frame
}

inline void RApplication::videoMove(int _fid)
{
	isSkip = true;
	//lastBumpfid = 0;
	capture.set(CV_CAP_PROP_POS_FRAMES,_fid);
	sensor->reset();
	hLineGraph->clear();
	vLineGraph->clear();
}

bool RApplication::keyboardCtrl(char btn)
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

void RApplication::writeVideo()
{
	char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
	_splitpath_s( Pubvar::videoPath.c_str(), drive, dir, fname, ext);
	
	string outputPath = "";
	int start = 0, end = 0, idx = 0, winsize = 8 * fps;
	VideoWriter writer;
	VideoCapture capCut;
	CV_Assert(capCut.open(Pubvar::videoPath));	
	Mat cutFrame;

	for (int i = winsize; i < videoLen; ++i)
	{		
		if(fidOI[i])
		{				
			outputPath = "data/" + (string)fname + "_" + to_string(idx++) + ext;
			writer = VideoWriter(outputPath, CV_FOURCC('D', 'I', 'V', '3') , fps, frame.size());
			capCut.set(CV_CAP_PROP_POS_FRAMES, i-winsize);			
			for(int j=i-winsize ; j<=i ; j++)
			{
				capCut >> cutFrame;
				writer << cutFrame;
			}
			writer.release();			
		}
	}	
}