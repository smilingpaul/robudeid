#ifndef __BlockMatchingSensor_
#define __BlockMatchingSensor_

#include "Sensor.h"

class ROBUDEID_EXPORTS BlockMatchingSensor : public Sensor
{
public:
	BlockMatchingSensor();
	~BlockMatchingSensor();
	virtual Point2d sense(const Mat& frame, int fid = 0);
	virtual void reset(int fid = 0);	

protected:
	virtual double horizontal();
	virtual double vertical();
	virtual int match(const Mat& v1, const Mat& v2, double thr);
	virtual void showTimeSlice();
	virtual void init_exec(const Mat& frame);

	Mat mTsiH,	//time slice image horizontal
		mTsiV;	//time slice image vertical

	Mat curHSlice,	//current frmae horizontal time slice
		lastHSlice,	//last frame horizontal time slice
		curVSlice,	//current frmae vertical time slice
		lastVSlice;	//last frame vertical time slice

	double tsiVUpperBoundRate,		//upper bound rate of vertical time slice image
		   tsiVLowerBoundRate,		//lower bound rate of vertical time slice image
		   tsiHRighterBoundRate,	//righter bound rate of horizontal time slice image
		   tsiHLefterBoundRate,		//lefter bound rate of horizontal time slice image
		   lineOI;				//interest line chosen from frame

	int row, col;	//row and column of frame 	
	int tsiVU,		//upper bound of vertical time slice image
		tsiVL,		//lower bound of vertical time slice image
		tsiHL,		//lefter bound of horizontal time slice image
		tsiHR;		//righter bound of horizontal time slice image

	int hSliceDistThr,
		vSliceDistThr;	//if slice diff is larger than threshold, diff is set to 0

	int lastH;	//horizontal movement of last iteration

private:
	bool isFirstScene;		//flag  of scene first time
	int tsiIdx;	//column index of time slice image when growing
	int tsiLen;		//length of time slice image
	
};

#endif	//#ifndef __BlockMatchingSensor_
