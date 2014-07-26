#include "CodebookGeneration.h"

int main()
{
	TrainBow::getSingleton()->go();
}

TrainBow* TrainBow::singleton = NULL;

TrainBow::TrainBow():
	imgPath("sample/training_img/"),
	bowVocSize(256)
{
	readSurfDescriptor();
}


TrainBow::~TrainBow()
{
}

TrainBow* TrainBow::getSingleton()
{
	if(!singleton)
		singleton = new TrainBow();
	
	return singleton;
}

void TrainBow::go()
{		
	cout<<"local feature extraction..."<<endl;
	this->readImgs(imgPath + "/hump/");
	this->readImgs(imgPath + "/anomaly/");
	this->writeSurfDescriptor();	
	cout<<"Bag of word clustering..."<<endl;
	this->trainVocabulary();
}

void TrainBow::readImgs(string path)
{		
	struct _finddata_t c_file;
	long hFile;	
	string filter = path + "*.jpg";		
	hFile = _findfirst(filter.c_str(), &c_file);	
	CV_Assert(hFile != -1);

	Mat img, objMask;
	do
	{		
		img = imread(path + c_file.name);			
		objMask = imread( path + "mask/" + c_file.name, IMREAD_GRAYSCALE);		
		this->surfExtractor(img, objMask);			
	}
	while(_findnext( hFile, &c_file )==0 );	
}

void TrainBow::surfExtractor(Mat img, Mat mask)
{
	vector<KeyPoint> cornor;
	Mat imgCornor, descriptor;	
	int minHessian = 400; //SURF Hessian Threshold
	Ptr<FeatureDetector> detector(new SurfFeatureDetector());
	Ptr<DescriptorExtractor> extractor(new SurfDescriptorExtractor(minHessian,4,2,false));
	detector->detect(img, cornor, mask);
	extractor->compute(img, cornor, descriptor);
	featuresUnclustered.push_back(descriptor);
}

void TrainBow::trainVocabulary()
{
	TermCriteria terminate_criterion;
    terminate_criterion.epsilon = FLT_EPSILON;
	BOWKMeansTrainer* bowTrainer = new BOWKMeansTrainer( this->bowVocSize, terminate_criterion, 3, KMEANS_PP_CENTERS );

	if(featuresUnclustered.rows > this->bowVocSize)
		dictionary = bowTrainer->cluster(featuresUnclustered);
		
	this->writeBowCodebook();	
}

bool TrainBow::readSurfDescriptor()
{	
	FileStorage fs("sample/imageDescriptor.yml", FileStorage::READ);
	if( fs.isOpened() )
    {		
        fs["imageDescriptor"] >> featuresUnclustered;	
		cout<<"load imageDescriptor ok!!"<<endl;
        return true;
    }
    return false;
}

bool TrainBow::writeSurfDescriptor()
{
	FileStorage fs("sample/imageDescriptor.yml", FileStorage::WRITE);
	if( fs.isOpened() )
    {
         fs << "imageDescriptor" << featuresUnclustered;
        return true;
    }
    return false;
}

bool TrainBow::writeBowCodebook()
{
	string dictPath = "sample/dictionary-" + to_string(bowVocSize) + ".yml";
	FileStorage fs(dictPath, FileStorage::WRITE);
	if( fs.isOpened() )
    {
         fs << "dictionary" << dictionary;
        return true;
    }
    return false; 
}