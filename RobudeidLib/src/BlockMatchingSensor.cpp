#include "BlockMatchingSensor.h"

BlockMatchingSensor::BlockMatchingSensor():
	tsiVUpperBoundRate(0.25),
	tsiVLowerBoundRate(0.75),
	tsiHRighterBoundRate(0.6),
	tsiHLefterBoundRate(0.33),
	tsiLen(1000),		//length of time slice image is 1000
	tsiIdx(0),
	lineOI(0.5),			//middle line
	hSliceDistThr(600),
	vSliceDistThr(600),	//if slice diff is larger than threshold, diff is set to 0
	lastH(0)
{
}

BlockMatchingSensor::~BlockMatchingSensor()
{
	mTsiH.release();
	mTsiV.release();
	curHSlice.release();
	lastHSlice.release();
	curVSlice.release();
	lastVSlice.release();
}

Point2d BlockMatchingSensor::sense(Mat frame)
{	
	if(curHSlice.empty())	
		init_exec(frame);

	//update time slice
	lastHSlice = curHSlice;
	lastVSlice = curVSlice;		
	curHSlice = frame.row(int(row * lineOI)).colRange(tsiHL, tsiHR).clone();	
	curVSlice = frame.col(int(col * lineOI)).rowRange(tsiVU, tsiVL).clone();
#ifdef DEMO
	showTimeSlice();
#endif
	return Point2d(horizontal(), vertical());		
}


double BlockMatchingSensor::horizontal()	//try other distance measure in the future. ex:°Ò«¢¹y¶ZÂ÷ Mahalanobis() 
{	
	int m = match(curHSlice.t(), lastHSlice.t(), hSliceDistThr);	
	return m*lastH >= 0 ? m : 0;
}

double BlockMatchingSensor::vertical()	//try other distance measure in the future. ex:°Ò«¢¹y¶ZÂ÷ Mahalanobis() 
{	
	return match(curVSlice, lastVSlice, vSliceDistThr);
}

/**
*	v1 and v2 must be vertical and same length
*	hill climbing
**/
int BlockMatchingSensor::match(const Mat& v1, const Mat& v2, double thr)	
{
	int r1 = maxShift,
		r2 = v1.rows-maxShift;

	double curDist = norm(v1.rowRange(r1,r2), v2.rowRange(r1,r2)),
		   lastDist,
		   dist;

	int i,j;
	
	//move upward
	for(i=1; i<=maxShift; ++i)
	{
		lastDist = curDist;
		curDist = norm(v1.rowRange(r1-i,r2-i), v2.rowRange(r1,r2));
		if(curDist >= lastDist)
			break;
	}

	dist = curDist;
	curDist = norm(v1.rowRange(r1,r2), v2.rowRange(r1,r2));
	int move = -1*(i-1) ;	//negative

	//move downward
	for(j=1; j<=maxShift; ++j)
	{
		lastDist = curDist;
		curDist = norm(v1.rowRange(r1+j,r2+j), v2.rowRange(r1,r2));
		if(curDist >= lastDist)
			break;
	}

	if(lastDist < dist)
	{
		dist = lastDist;
		move = j-1;
	}
		
	if(dist > thr)
		return 0;
	else
		return move;
}

void BlockMatchingSensor::showTimeSlice()
{
	if(tsiIdx == tsiLen)
		tsiIdx = 0;

	curHSlice.copyTo(mTsiH.row(tsiIdx));	//copy to horizontal time slice image
	curVSlice.copyTo(mTsiV.col(tsiIdx++));	//copy to vertical time slice image

	imshow("horizontal of time slice image",mTsiH(Range(0,tsiIdx),Range::all()));
	imshow("vertical of time slice image",mTsiV(Range::all(),Range(0,tsiIdx)));
}

void BlockMatchingSensor::reset()
{
	tsiIdx = 0;
}

void BlockMatchingSensor::init_exec(const Mat& frame)
{
	this->row = frame.rows;
	this->col = frame.cols;		
	tsiHR = int(col * tsiHRighterBoundRate);
	tsiHL = int(col * tsiHLefterBoundRate);
	tsiVL = int(row * tsiVLowerBoundRate);
	tsiVU = int(row * tsiVUpperBoundRate);
	mTsiH.create(tsiLen, tsiHR - tsiHL, CV_8UC3);	//initialize horizontal time slice image
	mTsiV.create(tsiVL - tsiVU  , tsiLen, CV_8UC3);	//initialize vertical time slice image
	
	//initialize last and current slice
	curHSlice = frame.row(int(row * lineOI)).colRange(tsiHL, tsiHR).clone();
	curVSlice = frame.col(int(col * lineOI)).rowRange(tsiVU, tsiVL).clone();
}

