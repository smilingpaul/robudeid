#include <iostream>
#include <fstream>
#include "Pubvar.h"
#include "DApplication.h"

using namespace std;

//initialize parameter
void initParm()
{
	//initialize by parm.ini
	fstream fin;
    fin.open("parm.ini", ios::in);	
	if(!fin)
		return;	//ini not found

	char buf[1000];	
	char* delim = "\t#= ";
	while(fin.getline(buf,sizeof(buf)))
	{		
		if(buf[0]=='#' || buf[0]=='\0' || (buf[0]=='[' && buf[strlen(buf)-1]==']'))	// [XXXX]
			continue;				
		char* tkn = strtok(buf,delim);		
		
		if(string(tkn) == "videoPath")				
			Pubvar::videoPath = strtok(NULL,delim);
		else if(string(tkn) == "dictPath")		
			Pubvar::dictPath = strtok(NULL,delim);	
		else if(string(tkn) == "SVMmodel")		
			Pubvar::SVMmodel = strtok(NULL,delim);	
		else if(string(tkn) == "nmeaPath")				
			Pubvar::nmeaPath = strtok(NULL,delim);
		else if(string(tkn) == "demoVideoPath")				
			Pubvar::demoVideoPath = strtok(NULL,delim);
		else if(string(tkn) == "sensorMethod")	
			Pubvar::sensorMethod = atoi(strtok(NULL,delim));		
		else if(string(tkn) == "sensitive")		
			Pubvar::sensitive = atoi(strtok(NULL,delim));	
		else if(string(tkn) == "ROI_UP")		
			Pubvar::ROI_UP = atof(strtok(NULL,delim));
		else if(string(tkn) == "ROI_DOWN")		
			Pubvar::ROI_DOWN = atof(strtok(NULL,delim));
		else if(string(tkn) == "ROI_LEFT")		
			Pubvar::ROI_LEFT = atof(strtok(NULL,delim));
		else if(string(tkn) == "ROI_RIGHT")		
			Pubvar::ROI_RIGHT = atof(strtok(NULL,delim));		
		else if(string(tkn) == "diOfError")		
			Pubvar::diOfError = atof(strtok(NULL,delim));
		else if(string(tkn) == "mdOfError")		
			Pubvar::mdOfError = atof(strtok(NULL,delim));
	}	
}

//robude videoPath
void main(int argc, char* argv[])
{	
	initParm();	//initialize parameter		
	if(argc>1)
		Pubvar::videoPath = argv[1];
	
	DApplication::getSingleton()->go();		
}