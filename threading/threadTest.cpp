#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdio.h>

#define INV_THRESHOLD 2

using namespace cv;

Mat frameIn;
boost::mutex mtx_;

void handleImageIn(int cameraNum)
{
  VideoCapture cap(cameraNum);
  if(!cap.isOpened())
  {
    std::cout << "Camera could not be found" << std::endl;
    return;
  }	
  while(1)
  {
    mtx_.lock();
    cap >> frameIn;
    std::cout << "Captured and within lock" << std::endl;
    mtx_.unlock();
    imshow("Input", frameIn);
//    if(waitKey(30) >= 0) 
//      break;
  }
  cap.release();
}

int main(int argc, char ** argv)
{
  namedWindow("Input",1);
  boost::thread camManage;
  namedWindow("Scharr", 1);
  namedWindow("Output",1); // Pattern output
  camManage = boost::thread(handleImageIn,0);
  Mat curFrame;
  Mat buffer;
  Mat out;
  Mat normPattern, invPattern;
  normPattern = imread("Patterns/ConventionalGray/08.bmp",1);
  if(normPattern.empty())
    std::cout << "Pattern not found" << std::endl;
  cvtColor(normPattern, normPattern, CV_BGR2GRAY);
  //normPattern = normPattern.t();
  //cv.Resize(normPattern, normPattern, interpolation
  //normPattern.resize(Size(320,240),0);
  Mat whiteMat = Mat::ones(normPattern.size(),normPattern.type())*255;
  subtract(whiteMat, normPattern, invPattern);
  //invPattern = normPattern;
  int patternCount = 0;
  bool invertFlag = false;
  while(1)
  {
    mtx_.lock();
    curFrame = frameIn.clone();
    mtx_.unlock();
    if(!curFrame.empty())
    {
      cvtColor(curFrame, buffer, CV_BGR2GRAY);
      medianBlur(buffer, buffer, 5);
      Scharr(buffer, out, CV_8U,0,1,1,20, BORDER_DEFAULT);
      imshow("Scharr", out);
    }
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
    if(waitKey(1) >= 0) 
      break;
  }
  return 0;
}

    
        
