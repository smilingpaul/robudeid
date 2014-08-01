#ifndef __DApplication_
#define __DApplication_

#include <fstream>
#include "BlockMatchingSensor.h"
#include "PhaseBasedSensor.h"
#include "GSensor.h"
#include "BDetector.h"
#include "Graph.h"
#include "Pubvar.h"
#include "ObjDetection.h"
#include "ObjRecog.h"

//#define RECORD_VIDEO

class DApplication
{
public:
	DApplication();
	~DApplication();

	static DApplication* getSingleton();
	void go();	
	
private:
	static DApplication* singleton;
	static void fidControler(int, void*);
	
	inline void processing();
	inline void videoRecorder(Mat& img);	//DEMO video
	inline void videoMove(int _fid);
	inline bool keyboardCtrl(char btn);

	int videoLen, fps, sec, fid, playSpeed;
	VideoWriter demoWriter;
	VideoCapture capture;
	bool isSkip, uneven;
	int *objType;
	
	Mat frame;
	Sensor* sensor;
	BDetector* bDtr;
	Graph* hLineGraph;
	Graph* vLineGraph;
	Rect rectROI;
	Mat ROI;
	ObjRecog* recog;
};

#endif	//#ifndef __DApplication_
