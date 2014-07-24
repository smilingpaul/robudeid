#ifndef __PhaseBasedSensor_
#define __PhaseBasedSensor_

#include "Sensor.h"

class ROBUDEID_EXPORTS PhaseBasedSensor : public Sensor
{
public:
	PhaseBasedSensor();
	~PhaseBasedSensor();
	virtual Point2d sense(const Mat& frame, int fid = 0); 
	
protected:	
	virtual void showSubFrame();
	virtual void init_exec(const Mat& frame);
	double lb, rb, ub, db;	//left, right, up, down bound
	double rsize, csize;	//sub frame size
	int row, col;
	Mat subFrame[9];	
};

#endif	//#ifndef __AreaSnesor_