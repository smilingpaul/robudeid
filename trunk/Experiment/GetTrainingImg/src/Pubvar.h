#ifndef __Pubvar_
#define __Pubvar_

#include <cstdio>
#include <string>

using namespace std;

class Pubvar
{
public:
	Pubvar();
	~Pubvar();

	static string videoPath, trainingImgPath;
	static double ROI_UP,		//object detection
				  ROI_DOWN,
				  ROI_LEFT,
				  ROI_RIGHT;

	static double diOfError,
				  mdOfError;
};


#endif	//#ifndef __Pubvar_