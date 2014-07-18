#ifndef __ObjDetection_
#define __ObjDetection_

#include "types.h"

#define BOUNDARY_255(x) ((x)<256 ? (x) : 255)
#define POWTRANSFORM(x,y,z) ((uchar)(255 * pow((double)(x)/(y), (z))))		//scale to 1~255
#define POLYTRANSFORM(x) BOUNDARY_255(((x)*(x)))

class ROBUDEID_EXPORTS ObjDetection
{
public:
	ObjDetection();
	~ObjDetection();
	static ObjDetection* getSingleton();
	
	void roadSegmentation(Mat src, Mat& dst = Mat());
	void objDetection(Mat src, Mat& dst);	
	void fill_hole(Mat src, Mat& dst);

private:
	static ObjDetection* singleton;
	static void aControler(int, void*);
	static void bControler(int, void*);
	static void cControler(int, void*);
	void showHist(Mat src);
	void principalConnectedComponent(Mat src, Mat& dst);	
	pair<int,int> triangleThr(Mat src, Mat mask= Mat());
	void calObj();

	Mat diDiff, moduleDiff, error, energy, mixtureModule, road;	//road detection
	double diOfError, mdOfError;

	Mat sobelDoubleThr, gxy, objProbality;	//object detection
	int a,b,c;	
	double minThr;
	pair<int,int> dIntensity;	//dominant intensity
};

#endif	//#ifndef __ObjDetection_