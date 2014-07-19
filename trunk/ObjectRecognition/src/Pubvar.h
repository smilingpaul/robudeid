#ifndef __Pubvar_
#define __Pubvar_

#include <cstdio>
#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define ANOMALY 1
#define HUMP 2

#define DEMO
//#define EXPERIMENT

using namespace std;
using namespace cv;

class Pubvar
{
public:
	Pubvar();
	~Pubvar();

	static string videoPath, dictPath, SVMmodel;
	static double ROI_UP,		//object detection
				  ROI_DOWN,
				  ROI_LEFT,
				  ROI_RIGHT,
				  INIT_ROAD_WINSIZE ,
				  OBJ_WINSIZE;

	static double diOfError,
				  mdOfError;

};


#endif	//#ifndef __Pubvar_