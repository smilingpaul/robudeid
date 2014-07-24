#ifndef __RApplication_
#define __RApplication_

#include <fstream>
#include "BlockMatchingSensor.h"
#include "PhaseBasedSensor.h"
#include "GSensor.h"
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

	//bump video
	bool* fidOI;

};

#endif	//#ifndef __RApplication_