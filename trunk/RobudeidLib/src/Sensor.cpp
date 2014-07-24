#include "Sensor.h"

Sensor::Sensor():
	maxShift(10)
{	
}

Sensor::~Sensor()
{
}

Point2d Sensor::sense(const Mat& frame, int fid)
{
	return Point2d(0,0);
}

void Sensor::reset(int fid)
{
}

Mat Sensor::BGR2CV_32FC1(Mat m)
{
	Mat _m;
	cvtColor(m, _m, CV_BGR2GRAY);	
	_m.convertTo(_m,CV_32FC1);	
	return _m;
}

void Sensor::init_exec(const Mat& frame)
{

}