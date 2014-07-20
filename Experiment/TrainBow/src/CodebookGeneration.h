#ifndef __TrainBow_
#define __TrainBow_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/features2d/features2d.hpp>  
#include <opencv2/nonfree/features2d.hpp>
#include <fstream>
#include <io.h>
#include <direct.h>
#include <vector>

using namespace std;
using namespace cv;

class TrainBow
{
public:
	TrainBow();
	~TrainBow();

	static TrainBow* getSingleton();
	void go();			

private:		
	static TrainBow* singleton;
	void surfExtractor(Mat img, Mat mask);	//extract surf and push into bag of word	
	void trainVocabulary();
	bool readSurfDescriptor();
	bool writeSurfDescriptor();
	bool writeBowCodebook();

	void readImgs(string path);
	string imgPath;
	Mat featuresUnclustered, dictionary;	//bag of word
	int bowVocSize;
};

#endif	//#ifndef __TrainBow_