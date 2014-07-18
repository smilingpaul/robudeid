#ifndef __Graph_
#define __Graph_

#include "types.h"

#define UNIT 10

class ROBUDEID_EXPORTS Graph
{
public:
	Graph();
	~Graph();
	void insert(int y);
	Mat update();
	void writeImg(string filename);
	void clear();

private:	
	Mat lineGraph;
	int idx;	
	vector<int> data;
	
};

#endif	//#ifndef __Graph_