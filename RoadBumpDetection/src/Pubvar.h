#ifndef __Pubvar_
#define __Pubvar_

#include <cstdio>
#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define BLOCK_MATCHING 1
#define PHASE_BASED	2

#define DEMO
//#define EXPERIMENT

using namespace std;
using namespace cv;

class Pubvar
{
public:
	Pubvar();
	~Pubvar();

	static string videoPath;
	static int sensitive,			   
			   sensorMethod;	

	static bool detectTurn;	//we dont use in this version	
};


#endif	//#ifndef __Pubvar_