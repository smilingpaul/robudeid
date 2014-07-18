#include <iostream>
#include <fstream>
#include "Pubvar.h"
#include "Rapplication.h"

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
	while(fin.getline(buf,sizeof(buf)))
	{		
		if(buf[0]=='#' || buf[0]=='\0' || (buf[0]=='[' && buf[strlen(buf)-1]==']'))	// [XXXX]
			continue;		
		char* delim = "\t#= ";
		char* tkn = strtok(buf,delim);		
		
		if(string(tkn) == "videoPath")				
			Pubvar::videoPath = strtok(NULL,delim);
		else if(string(tkn) == "sensorMethod")	
			Pubvar::sensorMethod = atoi(strtok(NULL,delim));		
		else if(string(tkn) == "sensitive")		
			Pubvar::sensitive = atoi(strtok(NULL,delim));		
	}	
}

//robude videoPath
void main(int argc, char* argv[])
{	
	initParm();	//initialize parameter		
	if(argc>1)
		Pubvar::videoPath = argv[1];
	
	RApplication::getSingleton()->go();	
}