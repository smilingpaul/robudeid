//#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <cstdio>
#include "Pubvar.h"
#include "GTIApplication.h"

//initialize by parm.ini
void initParm()
{			
	fstream fin;
    fin.open("parm.ini", ios::in);	
	if(!fin)
		return;	//ini not found

	char buf[10000];	
	while(fin.getline(buf,sizeof(buf)))
	{		
		//cout<<buf<<endl;
		if(buf[0]=='#' || buf[0]=='\0' || (buf[0]=='[' && buf[strlen(buf)-1]==']'))	// [XXXX]
			continue;		
		char* delim = "\t#= ";
		char* tkn = strtok(buf,delim);		
		if(string(tkn) == "videoPath")		
			Pubvar::videoPath = strtok(NULL,delim);		
		else if(string(tkn)=="trainingImgPath")
			Pubvar::trainingImgPath = strtok(NULL,delim);	
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

int main( int argc, char** argv )
{	
	initParm();
	if(argc>1)
		Pubvar::videoPath = argv[1];

	GTIApplication::getSingleton()->go();

}

