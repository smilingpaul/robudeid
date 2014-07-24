#ifndef __Sensor_
#define __Sensor_

#include "types.h"

#define SURPRESS(x) ((x)*(x)>maxShift*maxShift ? maxShift: (x))	// let |x| <= |maxShift|
#define SURPRESS2(x) ((x)*(x)>maxShift*maxShift ? 0: (x)) 

class ROBUDEID_EXPORTS Sensor
{
public:
	Sensor();
	~Sensor();
	virtual Point2d sense(const Mat& frame, int fid = 0);
	virtual void reset(int fid = 0);

protected:
	virtual void init_exec(const Mat& frame);
	Mat BGR2CV_32FC1(Mat m);
	int maxShift;	
};

#endif	//#ifndef __Sensor_