#include "Pubvar.h"

//initialize by default	
string Pubvar::videoPath ="test1.avi";
string Pubvar::dictPath = "dictionary.yml";
string Pubvar::SVMmodel = "bow.m";
string Pubvar::nmeaPath = "test1,nmea";
string Pubvar::demoVideoPath = "sample/demo.avi";
int Pubvar::sensitive = 6;
int Pubvar::sensorMethod = BLOCK_MATCHING;
bool Pubvar::detectTurn = true;
double Pubvar::ROI_UP = 0.6;
double Pubvar::ROI_DOWN = 0.95;
double Pubvar::ROI_LEFT = 0.3;
double Pubvar::ROI_RIGHT = 0.7;
double Pubvar::INIT_ROAD_WINSIZE  = 4;
double Pubvar::OBJ_WINSIZE = 4;
double Pubvar::diOfError = 2.5;
double Pubvar::mdOfError = 5;