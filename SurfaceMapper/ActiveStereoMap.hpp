#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "ProcGen.hpp"

using namespace cv;
class ActiveStereoMap
{
public:
  ActiveStereoMap(VideoCapture c, Size projSize);
  void runMapping(int levels);
  Mat computeDisparity(Mat f);
  Mat getGraymap(void);
  Mat getGrayComposite(void);
private:
  VideoCapture cap;
  Size patternSize;
  Size streamSize;
  Vector<Mat> vimgs;
  Vector<Mat> himgs;
  Mat grayImg;
  Mat grayPattern;
  
  int intPow(int base, unsigned int power);
  void processRawImage(Mat rawImg, Mat destImg, int thresh, int factor);
  void capturePattern(Mat tempPattern, int levels, int level);
};