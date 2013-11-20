#ifndef GRAY_PROJ_H
#define GRAY_PROJ_H

#include <opencv/opencv.hpp>
#include <vector>
#include <iostream>

#define RES_W 848
#define RES_H 480

using namespace cv
public class GrayProjector
{
public:
  GrayProjector();
  virtual ~GrayProjector();
  Mat modulateImage(Mat inImg, int pattern, bool phase);
  
private:
  int imWidth, imHeight;
  Mat payloadBuffer;
  Mat outputBuffer;
  vector<Mat> grayPatterns;
  Mat getPattern(int patternCode);
};

#endif