#ifndef __BDetector_
#define __BDetector_

#include "types.h"

#define NOBUMP 0
#define CONVEX 1
#define CONCAVE 2
#define BUMP 3

//#define prob

#define for_each_nrEvent(i) for(i = 0; i<3; ++i)
/* bump detector */
class ROBUDEID_EXPORTS BDetector
{
public:
	BDetector();
	BDetector(int fps, int thr, bool _detectTurn = true);
	~BDetector();
	//=======================================================================================
	void update(Point2d data);
	int isBump();	// naive algorithm, but it can classify concave or convex
	//=======================================================================================
	void update2(Point2d data);	// faster algorithm, but can not classify concave or convex
	bool isBump2();	// faster algorithm, but can not classify concave or convex	
	//=======================================================================================
	double updateBumpEvt(pair<double,double>  label_pr);	
	bool barExist();
	void clear();
	void evtClear();

protected:	
	size_t detectionWinSize, recogWinSize;
	int maxShift, bumpThr, bumpinSize;	
	bool detectTurn;

	int *distribution;	//data distribution, for finding min max
	int curMax, curMin;	//vertical max/min movement
	deque<int> xData, yData;	//sliding window
	int xArea;	

	double nrEvent[4];
#ifdef prob	
	deque< pair<double,double> > tagQue;
#else
	deque<int> tagQue;
#endif
	
	vector<double> prBar;
};

#endif	//#ifndef __BDetector_
