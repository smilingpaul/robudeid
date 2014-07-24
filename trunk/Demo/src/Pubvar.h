#ifndef __Pubvar_
#define __Pubvar_

#include <cstdio>
#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define BLOCK_MATCHING 1
#define PHASE_BASED	2
#define G_SENSOR	3

#define ANOMALY 1
#define HUMP 2

using namespace std;
using namespace cv;

class Pubvar
{
public:
	Pubvar();
	~Pubvar();

	static string videoPath, nmeaPath, dictPath, SVMmodel, demoVideoPath;
	static int sensitive,			   
			   sensorMethod;	

	static bool detectTurn;	//we dont use in this version	

	/**object detection*/
	static double ROI_UP,		
				  ROI_DOWN,
				  ROI_LEFT,
				  ROI_RIGHT,
				  INIT_ROAD_WINSIZE ,
				  OBJ_WINSIZE;

	static double diOfError,
				  mdOfError;
};


#endif	//#ifndef __Pubvar_