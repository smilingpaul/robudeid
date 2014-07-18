#ifndef __ObjRecog_
#define __ObjRecog_

#include "types.h"
#include <linear.h>

//#define SHOW_CONFIDENCE
#define bound_check(x) ( double(((int(x*100))<0||(int(x*100))>100) ? 0.5 : x) ) 

class ROBUDEID_EXPORTS ObjRecog
{
public:
	ObjRecog(char* SVMmodel, char* dictPath);
	~ObjRecog();		
	pair<double,double> objEstimate(Mat img, Mat mask);
	pair<double,double> do_predict(double* feature);
	double* featureExtraction(Mat img, Mat mask);

	bool loadBowDict(char* dictPath);

private:		
	Mat dictionary;	//bag of word dictionary
	int minHessian;	//SURF Hessian Threshold

	int max_nr_attr, nr_class, nr_feature;
	struct model* model_;
	struct feature_node *fnode;
	bool flag_predict_probability;	
};


#endif	//#ifndef __ObjRecog_