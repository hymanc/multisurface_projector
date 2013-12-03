#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdio.h>

#include "ProcGen.hpp"

#define INV_THRESHOLD 2

using namespace cv;

int main(int argc, char ** argv)
{
  VideoCapture cap(1);
  if(!cap.isOpened())
    return -1;
  namedWindow("Scharr", 1);
  namedWindow("Input",1);
  namedWindow("Output", CV_WINDOW_NORMAL); // Pattern output
  namedWindow("Gray", 1);
  
  Mat buffer;
  Mat out;
  Mat normPattern, invPattern;
  normPattern = imread("Patterns/ConventionalGray/08.bmp",1);
  cvtColor(normPattern, normPattern, CV_BGR2GRAY);
  //normPattern = normPattern.t();
  //cv.Resize(normPattern, normPattern, interpolation
  //normPattern.resize(Size(320,240),0);
  Mat whiteMat = Mat::ones(normPattern.size(),normPattern.type())*255;
  subtract(whiteMat, normPattern, invPattern);
  invPattern = normPattern;
  setWindowProperty("Output", CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);
  imshow("Output",normPattern);
  int patternCount = 0;
  bool invertFlag = false;
  while(1)
  {
    Mat frame;
    cap >> frame;
    cvtColor(frame, buffer, CV_BGR2GRAY);
    medianBlur(buffer, buffer, 5);
    Scharr(buffer, out, CV_8U,0,1,1,20, BORDER_DEFAULT);
    imshow("Scharr", out);
    imshow("Input", frame);
    patternCount ++;
    if(patternCount > INV_THRESHOLD)
    {
      invertFlag = !invertFlag;
      patternCount = 0;
    }
    if(invertFlag)
    {
      imshow("Output", invPattern);
    }
    else
    {
      imshow("Output", normPattern);
    }
    if(waitKey(30) >= 0) 
      break;
  }
  cap.release();
  return 0;
}
        
