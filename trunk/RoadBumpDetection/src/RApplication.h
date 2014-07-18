#ifndef __RApplication_
#define __RApplication_

#include <fstream>
#include "BlockMatchingSensor.h"
#include "PhaseBasedSensor.h"
#include "BDetector.h"
#include "Graph.h"
#include "Pubvar.h"

class RApplication
{
public:
	RApplication();
	~RApplication();

	static RApplication* getSingleton();
	void go();	

private:
	static RApplication* singleton;
	static void fidControler(int, void*);
	
	void writeVideo();
	inline void processing();
	void expProcessing();	
	inline void videoMove(int _fid);
	bool keyboardCtrl(char btn);

	int videoLen, fps, sec, fid, playSpeed;
	VideoCapture capture;
	bool isSkip, uneven;
	//int bumpClass, lastBumpfid;
	
	Mat frame;
	Sensor* sensor;
	BDetector* bDtr;
	Graph* hLineGraph;
	Graph* vLineGraph;
	Rect rectROI;
	Mat ROI;

	//bump video
	bool* fidOI;

};

#endif	//#ifndef __RApplication_