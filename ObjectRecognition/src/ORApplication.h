#ifndef __ORapplication_
#define __ORapplication_

#include <fstream>
#include "Pubvar.h"
#include "ObjDetection.h"
#include "ObjRecog.h"

#define BUMP_STATE	1
#define OBJ_DETECTION_STATE	2

class ORapplication
{
public:
	ORapplication();
	~ORapplication();

	static ORapplication* getSingleton();
	void go();	

private:
	static ORapplication* singleton;
	static void fidControler(int, void*);
		
	inline void processing();	
	inline void videoMove(int _fid);
	bool keyboardCtrl(char btn);
	
	int videoLen, fps, sec, fid, playSpeed;
	VideoCapture capture;

	Mat frame;
	Rect rectROI;
	Mat ROI;
};

#endif	//#ifndef __ORapplication_