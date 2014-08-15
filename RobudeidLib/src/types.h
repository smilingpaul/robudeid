#ifndef __types_
#define __types_

#ifdef _WINDLL
#  define ROBUDEID_EXPORTS __declspec(dllexport)
#else
#  define ROBUDEID_EXPORTS
#endif

#define ANOMALY 1
#define HUMP 2

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;
using namespace cv;
#endif //__types_