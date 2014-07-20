#ifndef __GTIApplication_
#define __GTIApplication_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/features2d/features2d.hpp>  
#include <opencv2/nonfree/features2d.hpp>
#include <fstream>
#include <io.h>
#include <direct.h>
#include <vector>

#include "Pubvar.h"
#include "ObjDetection.h"

using namespace std;
using namespace cv;

class GTIApplication
{
public:
	GTIApplication();
	~GTIApplication();

	static GTIApplication* getSingleton();
	void go();		

private:	
	static void fidControler(int, void*);
	void processing();
	bool keyboardCtrl(char btn);
	void saveImage(int);

	static GTIApplication* singleton;

	VideoCapture capture;
	int videoLen, fps;	//video info
	int playSpeed, fid, sec;	//video player info
	Mat frame, obj, objMask;
	Rect rectROI;
	Mat ROI;
	int bar_file_idx, other_file_idx; //training image index
};

#endif	//#ifndef __GTIApplication_