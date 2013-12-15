#ifndef ACTIVE_STEREO_MAP
#define ACTIVE_STEREO_MAP

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "ProcGen.hpp"
#include "gray_lookup.hpp"

using namespace cv;
class ActiveStereoMap
{
  
public:
  ActiveStereoMap(VideoCapture c, Size projSize);
  void runMapping(int levels);
  Mat computeDisparity(Mat dCam, Mat dProj, Mat R, Mat T);
  Mat getGrayH(void);
  Mat getGrayV(void);
  Mat getGraymap(void);
  Mat getGrayProjH(void);
  Mat getGrayProjV(void);
  Mat getGrayComposite(void);
  Mat grayFilter(Mat src, int graylvl);
  static unsigned int grayToBinary(unsigned int graylvl);
  static Vec2b findGrayCoordinates(int graylvlH, int graylvlV, int nlvls, Size mapSize);
  static int intPow(int base, unsigned int power);
  void showWhite(void);
  
private:
  VideoCapture cap;
  Size patternSize;
  Size streamSize;
  
  Mat grayImg;
  Mat grayPattern;
  
  Mat grayH;
  Mat grayV;
  Mat grayProjH;
  Mat grayProjV;
  
  int numLevels;
  
  uint stripeSize;
  
  int * hLUT;
  int * vLUT;
  
  Rect roi;
  
  void processRawImage(Mat rawImg, Mat destImg, int thresh, int factor);
  void capturePattern(Mat tempPattern, Mat tempMat, int levels, int level, bool horizontalFlag);
  void generateGrayLuts(void);
  void getPatternROI(Mat src, Mat dst);
  Mat getWindow(int x, int y, int size);
};

#endif