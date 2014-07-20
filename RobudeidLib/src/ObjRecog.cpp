#include "ObjRecog.h"

ObjRecog::ObjRecog(const char* SVMmodel, const char* dictPath):
	minHessian(400),
	flag_predict_probability(false),
	max_nr_attr(512)
{	
	CV_Assert(loadBowDict(dictPath));
	CV_Assert(model_ = load_model(SVMmodel));
	nr_class = get_nr_class(model_);	
	nr_feature = get_nr_feature(model_);	

	if( model_->param.solver_type == L1R_LR ||		//L1-regularized logistic regression
		model_->param.solver_type == L2R_LR_DUAL)	//L2-regularized logistic regression (dual)
		flag_predict_probability = true;
}

ObjRecog::~ObjRecog()
{
	free_and_destroy_model(&model_);
}

bool ObjRecog::loadBowDict(const char* dictPath)
{
	FileStorage fs(dictPath, FileStorage::READ);
	if( fs.isOpened() )
    {
         fs["dictionary"] >> dictionary;
        return true;
    }
    return false; 
}

pair<double,double> ObjRecog::objEstimate(Mat img, Mat mask)
{
	CV_Assert(img.size() == mask.size());
	int flag;
	Mat obj, objMask, localFeatureMask;
	Mat objEl = getStructuringElement( MORPH_RECT, Size(7, 7)),
		localfeatureEl = getStructuringElement( MORPH_RECT, Size(5, 5));

	dilate( mask, objMask, objEl);
	dilate( mask, localFeatureMask, localfeatureEl);
	img.copyTo(obj, objMask);
		
	double* bowFeature = featureExtraction(obj, localFeatureMask, flag);
	//double* bowFeature = featureExtraction(img, mask, flag);
	if(flag)	//cornor less than 10
		return make_pair(ANOMALY, 0.5);
	else
		return do_predict(bowFeature);
}

double* ObjRecog::featureExtraction(Mat img, Mat mask, int& flag)
{
	Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);	
	Ptr<FeatureDetector> detector(new SurfFeatureDetector());
	Ptr<DescriptorExtractor> extractor(new SurfDescriptorExtractor(minHessian, 4, 2, false));
	BOWImgDescriptorExtractor bowDE(extractor,matcher);
	bowDE.setVocabulary(dictionary);
	vector<KeyPoint> cornor;
	Mat bowDescriptor;	
	double *feature = new double[nr_feature];
	detector->detect(img, cornor, mask);	
	flag = false;

	/*Mat mCornor;
	drawKeypoints(img, cornor,  mCornor, Scalar::all(-1),DrawMatchesFlags::DEFAULT);  
	imshow("mCornor", mCornor);*/

	if(cornor.size()>10)
	{
		bowDE.compute(img, cornor, bowDescriptor);	// 0~1	
		bowDescriptor *= 100;
		bowDescriptor.convertTo(bowDescriptor, CV_8U);
		for(int i=0; i<nr_feature; ++i)
			feature[i] = (double)bowDescriptor.at<uchar>(0,i);
	}
	else
	{
		for(int i=0; i<nr_feature; ++i)
			feature[i] = 0;
		flag = true;
	}
								
	return feature;
}

pair<double,double> ObjRecog::do_predict(double* feature)
{	
	fnode = (struct feature_node *) malloc(max_nr_attr*sizeof(struct feature_node));

	if(flag_predict_probability && !check_probability_model(model_))
	{
		fprintf(stderr, "probability output is only supported for logistic regression\n");
		exit(1);			
	}
		
	for(int i=0; i<nr_feature; ++i)
	{
		if(i>=max_nr_attr-2)	// need one more for index = -1
		{
			max_nr_attr *= 2;
			fnode = (struct feature_node *) realloc(fnode,max_nr_attr*sizeof(struct feature_node));
		}		
		fnode[i].index = i+1;
		fnode[i].value = feature[i];
	}

	fnode[nr_feature].index = -1;
	if(model_->bias>=0)
	{
		fnode[nr_feature].index = nr_feature + 1;
		fnode[nr_feature].value = model_->bias;
		fnode[nr_feature+1].index = -1;
	}

	double *prob_estimates=NULL;	
	double predict_label = 0;
	pair<double,double> predict_label_pr;
	if(flag_predict_probability)
	{
		prob_estimates = (double *) malloc(nr_class*sizeof(double));
		predict_label = predict_probability(model_, fnode, prob_estimates);

#ifdef SHOW_CONFIDENCE
		for(int i=0;i<model_->nr_class;i++)		
			cout<<(int)(prob_estimates[i] * 100) <<"%\t";								
		cout<<endl;	
#endif						
		predict_label_pr = make_pair(predict_label, prob_estimates[(int)predict_label-1]);
		free(prob_estimates);
	}
	else	
	{
		predict_label = predict(model_, fnode);		
		predict_label_pr = make_pair(predict_label, 1);
	}
	
	return predict_label_pr;
}