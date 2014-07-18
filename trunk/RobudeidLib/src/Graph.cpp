#include "Graph.h"

Graph::Graph():
	lineGraph(2*UNIT*10 +1, 500, CV_8UC3,Scalar(0,0,0)),	//rows: 20*maxsihft
	idx(0)
{
	//lineGraph.create(2*UNIT*Pubvar::maxShift +1, 500, CV_8UC3);	
}

Graph::~Graph()
{
	lineGraph.release();
}

void Graph::writeImg(string filename)
{
	imwrite(filename, lineGraph(Range::all(),Range(0,idx)));
}
void Graph::insert(int y)
{
	if(idx == lineGraph.cols-1)
		this->clear();

	data.push_back(y);
	++idx;
}

Mat Graph::update()
{
	if(idx>1)
	{
		Point p1(idx-2, lineGraph.rows/2 - data.at(idx-2)*UNIT ) ,
			p2(idx-1, lineGraph.rows/2 - data.at(idx-1)*UNIT ) ;
		line(lineGraph, p1, p2, CV_RGB(255,0,0));
	}
	return lineGraph;
}

void Graph::clear()
{
	lineGraph = Scalar::all(0);
	data.clear();
	idx=0;
}