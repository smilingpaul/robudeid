#include "PhaseBasedSensor.h"

PhaseBasedSensor::PhaseBasedSensor():
	lb(0.25),
	rb(0.75),
	ub(0.25),
	db(0.75),
	row(0),
	col(0),
	rsize(0),
	csize(0)
{			
}

PhaseBasedSensor::~PhaseBasedSensor()
{
	for(int i=0; i<9; ++i)
		subFrame[i].release();
}

Point2d PhaseBasedSensor::sense(Mat frame)
{		
	int sItr = 0;
	double dx[9], dy[9];
	double sumX = 0, sumX2 = 0, sumY = 0, sumY2 = 0, xSd = 0, ySd = 0;
	if(subFrame[0].empty())	
		init_exec(frame);	

	for(int i=0; i<3; ++i)		// compute phase correlation of sub-frame of 3 by 3 grid and its standard deviation	
		for(int j=0; j<3; ++j)		
		{
			//Mat _subFrame = frame.rowRange(ub*row + i*rsize, ub*row + (i+1)*rsize).colRange(lb*col + j*csize, lb*col + (j+1)*csize).clone();
			Mat _subFrame = frame(Range(ub*row + i*rsize, ub*row + (i+1)*rsize),Range(lb*col + j*csize, lb*col + (j+1)*csize)).clone();
			Point2d shift(0);	//set zero if execute first
			if(!subFrame[sItr].empty())
				shift = phaseCorrelate( BGR2CV_32FC1(_subFrame) , BGR2CV_32FC1(subFrame[sItr]) );

			dx[sItr] = shift.x;
			dy[sItr] = shift.y;
			sumX += dx[sItr];		//standard deviation
			sumX2 += dx[sItr] * dx[sItr];
			sumY += dy[sItr];
			sumY2 += dy[sItr] * dy[sItr];
			subFrame[sItr].release();
			subFrame[sItr++] = _subFrame;
		}		
	
	xSd = sqrt(sumX2/9 - sumX/9 * sumX/9);	//horizontal standard deviation in 3 by 3 grid
	ySd = sqrt(sumY2/9 - sumY/9 * sumY/9);	//vertical standard deviation in 3 by 3 grid
	nth_element(dx,dx+4,dx+9);	//find the median of horizontal movement
	nth_element(dy,dy+4,dy+9);	//find the median of vertical movement
	double mx = dx[4], my = dy[4];	//median of horizontal and vertical movement
	int xOK = 9, yOK = 9;	//number of non-outlier
	for(int i=0;i<9; ++i)	//remove outlier
	{
		if(mx - dx[i] > xSd || dx[i]-mx >xSd)	//horizontal outlier
		{
			sumX -= dx[i];
			--xOK;
		}
		if(my - dy[i] > ySd || dy[i]-my >ySd)	//vertical outlier
		{
			sumY -= dy[i];
			--yOK;
		}
	}	
	
	return Point2d(-1*SURPRESS(sumX/xOK), -1*SURPRESS(sumY/yOK));		
}

void PhaseBasedSensor::showSubFrame()
{
	for(int i=0; i<9; ++i)
		imshow(to_string(i), subFrame[i]);		
}

void PhaseBasedSensor::init_exec(const Mat& frame)
{
	this->row = frame.rows;
	this->col = frame.cols;
	this->rsize = this->row*(db-ub)/3;
	this->csize = this->col*(rb-lb)/3;
}