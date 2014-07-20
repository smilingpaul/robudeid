#ifndef __SVMformat_
#define __SVMformat_

#include <iostream>
#include <fstream>
#include <cstdio>
#include <io.h>
#include <direct.h>
#include <opencv/cv.h>
#include <opencv/highgui.h> 
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>

#define ANOMALY 1
#define HUMP 2

using namespace std;
using namespace cv;

class SVMformat
{
public:
	SVMformat();
	~SVMformat();

	static SVMformat* getSingleton();
	void go();			

private:		
	static SVMformat* singleton;	

	void bowExtractor(Mat img, Mat mask);
	void writeLibSVMformat(int label);
	bool readBowCodebook();
	void readImgs(string path);
	string imgPath;
	Mat dictionary;	//bag of word
	int bowVocSize;
	Mat bowDescSet; 
};

#endif  //#ifndef __SVMformat_
