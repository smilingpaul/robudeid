#include "GSensor.h"

GSensor::GSensor(string nmeaPath):
	gSize(1600),
	gsRate(30/5),			//未來視情況改成不要寫死...
	fps(30),				//未來視情況改成不要寫死...,
	gx(0),
	gy(0),
	gz(0)
{
	cout<<nmeaPath<<endl;
	fNmea.open(nmeaPath);
	CV_Assert(fNmea);	//nmea file does not exist	
	initNmea();	
}

GSensor::~GSensor()
{
	fNmea.close();
}

void GSensor::initNmea()
{

	char buf[1000];	
	char* delim = "\t, ";
	vx.reserve(gSize);	
	vy.reserve(gSize);
	vz.reserve(gSize);

	while(fNmea.getline(buf,sizeof(buf)))
	{		
		char* tkn = strtok(buf,delim);		
		if(tkn && string(tkn) == "$GSENSOR")	
		{	
			vx.push_back(atoi(strtok(NULL,delim)));
			vy.push_back(atoi(strtok(NULL,delim)));
			vz.push_back(atoi(strtok(NULL,delim))-900);
		}
	}	
}

Point2d GSensor::sense(const Mat& frame, int fid)
{	
	int i = fid/gsRate >= vx.size()? vx.size()-1 : fid/gsRate;
	return 	Point2d((vy.at(i))/25, (vz.at(i))/50);
}
