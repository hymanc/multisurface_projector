#ifndef GRAY_PROJ_H
#define GRAY_PROJ_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>

#define RES_W 640
#define RES_H 480


using namespace std;
using namespace cv;
class GrayProjector
{
public:
  GrayProjector();
  //virtual ~GrayProjector();
  Mat getGrayPattern(int scale, bool h);
private:
  vector<Mat> grayPatternsH;
  vector<Mat> grayPatternsV;
  string to_string(int _Val);
};

#endif