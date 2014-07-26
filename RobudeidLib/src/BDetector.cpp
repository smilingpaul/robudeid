#include "BDetector.h"

BDetector::BDetector():
	maxShift(10)
{
}

BDetector::BDetector(int fps, int thr, bool _detectTurn):
	maxShift(10),
	bumpThr(thr),
	detectionWinSize(fps * 1.5),	//bump detection winsize is 1.5 second
	recogWinSize(fps * 0.2),
	detectTurn(_detectTurn),
	curMax(INT_MIN),
	curMin(INT_MAX),
	xArea(0)
{	
	distribution = new int[2*maxShift +1];
	for(int i=0; i<2*maxShift +1; i++)
		distribution[i] = 0;
	
	int i;
	for_each_nrEvent(i)
		nrEvent[i] = 0;
}

BDetector::~BDetector()
{
}

void BDetector::update(Point2d data)
{
	xArea += abs(data.x);
	xData.push_back((int)data.x);
	if(xData.size() > detectionWinSize)
	{
		xArea -= abs(xData.front());
		xData.pop_front();
	}
	//update vertical
	yData.push_back((int)data.y);
	if(yData.size() > detectionWinSize)
		yData.pop_front();
}

// naive algorithm, but it can classify concave or convex
int BDetector::isBump()
{
	int bumpClass = BUMP;	
	curMax = INT_MIN; 
	curMin = INT_MAX;
	for(int i=0; i<yData.size(); ++i)
	{
		curMax = yData[i]>curMax ? yData[i] : curMax;
		curMin = yData[i]<curMin ? yData[i] : curMin;

		if(yData[i] > bumpThr/2)
			bumpClass = CONVEX;
		else if(yData[i] < -bumpThr/2)
			bumpClass = CONCAVE;			
	}	
	
	bool isturn = xArea > 3*detectionWinSize ? true : false;
	isturn = this->detectTurn && isturn;	//check if using turn detection	
	
	if( !isturn  &&(curMax - curMin)>= bumpThr)	//uneven threshold	&& curMax and curMin positive(negative) and negative(positive)
		return bumpClass;	
	else
		return NOBUMP;
}

// faster algorithm, but can not classify concave or convex
void BDetector::update2(Point2d data)
{
	//update horizontal
	xArea += abs(data.x);
	xData.push_back((int)data.x);
	if(xData.size() > detectionWinSize)
	{
		xArea -= abs(xData.front());
		xData.pop_front();
	}
	
	//update vertical
	bool recomputeMin = false, recomputeMax = false;
	int shift = (int)data.y + maxShift;	//range from 0~2*maxShift
		
	curMax = shift > curMax ? shift : curMax;	//update current max and min
	curMin = shift < curMin ? shift : curMin;
	
	++distribution[shift];
	yData.push_back(shift);

	if(yData.size() > detectionWinSize)	//if popped one is the maximal or minimal
	{
		if(--distribution[yData.front()]==0 )	
		{
			if(curMax==yData.front())
				recomputeMax = true;
			else if(curMin==yData.front())
				recomputeMin = true;
		}
		yData.pop_front();
	}
	int maxIdx = curMax, minIdx = curMin;	//search from last maximal/minimal
	if(recomputeMax)
	{
		while(distribution[--maxIdx]==0);
		curMax = maxIdx;
	}
	else if(recomputeMin)
	{
		while(distribution[++minIdx]==0);
		curMin = minIdx;
	}
}

// faster algorithm, but can not classify concave or convex
bool BDetector::isBump2()
{
	//cout<<curMax-10<<"\t"<<curMin-10<<"\t"<<xArea<<endl;
	bool isturn = xArea > 3*detectionWinSize ? true : false;
	isturn = this->detectTurn && isturn;	//check if using turn detection	
	if( !isturn  /*&& (curMax-maxShift) * (curMin-maxShift) <0 */&&(curMax - curMin)>= bumpThr)	//uneven threshold	&& curMax and curMin positive(negative) and negative(positive)
		return true;	
	else
		return false;
}

double BDetector::updateBumpEvt(pair<double,double>  label_pr)
{
	
#ifdef prob	
	nrEvent[(int)label_pr.first] += label_pr.second;
	tagQue.push_back(label_pr);
#else
	tagQue.push_back(label_pr.first);
	++nrEvent[(int)label_pr.first];	
#endif

	if(tagQue.size() > recogWinSize)
	{
#ifdef prob	
		nrEvent[(int)tagQue.front().first] -= (int)tagQue.front().second;
#else
		--nrEvent[tagQue.front()];
#endif
		tagQue.pop_front();
	}
	
	if(tagQue.size() > recogWinSize/3)
	{
		//cout<< (int)nrEvent[1] * 100 / tagQue.size()<<" "<< (int)nrEvent[2] *100 / tagQue.size()<<endl;	
		//cout<<(int)nrEvent[2] *100 / tagQue.size() <<"%"<<endl;
		prBar.push_back(nrEvent[HUMP] / tagQue.size());	
		return nrEvent[HUMP] / tagQue.size();
	}
	return 0;
}

bool BDetector::barExist()
{
	for(int i=0; i <prBar.size(); ++i)
		if(prBar.at(i) > 0.7)	//7%
			return true;

	return false;
}

void BDetector::clear()
{
	xArea = 0;
	curMin = 0;
	curMax = 0;
	xData.clear();
	yData.clear();	
	this->evtClear();
}

void BDetector::evtClear()
{
	int i;
	for_each_nrEvent(i)
		nrEvent[i] = 0;

	tagQue.clear();
	prBar.clear();
}