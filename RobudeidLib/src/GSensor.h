#ifndef __GSensor_
#define __GSensor_

#include "types.h"
#include "Sensor.h"
#include <fstream>

#define GTRANSFORMY(x) ((x)/32)	//huristic...
#define GTRANSFORMZ(x) (((x)-800)/50)	//huristic...

class ROBUDEID_EXPORTS GSensor : public Sensor
{
public:
	GSensor(string sNmea);
	~GSensor();
	virtual Point2d sense(const Mat& frame, int fid);

protected:			
	virtual void initNmea();

	fstream fNmea;
	//vector<int> gx,gy,gz;	
	vector<int> vx,vy,vz;	//displacement
	size_t gSize;
    int gsRate;	//gsensor sampling rate 
	int fps;

	int gx,gy,gz; //velocity
};

#endif	//#ifndef __GSensor_