#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;
int main(int argc, char ** argv)
{
  int cam = atoi(argv[1]);
  VideoCapture cap(cam);
  if(!cap.isOpened())
  {
    cout << "No camera connected at specified index" << endl;
    return -1;
  }
  Mat currentFrame;
  Mat processMat;
  Mat c1mat;
  vector<Mat> channels;
  namedWindow("Input",1);
  namedWindow("Normalized",1);
  namedWindow("Filter",1);
  cap >> currentFrame;
  waitKey(400);
  while(1)
  {
    cap >> currentFrame;
    imshow("Input", currentFrame);
    Rect roi(161,81,350,240);
    if(currentFrame.size().width > 0)
    {
      processMat = currentFrame(roi);
      resize(processMat,processMat,Size(700,480),CV_INTER_LINEAR);
      medianBlur(processMat, processMat, 5); // Median blur to reduce camera noise
      cvtColor(processMat, processMat, CV_BGR2YCrCb);
      split(processMat, channels);
      equalizeHist(channels[0], channels[0]);
      merge(channels, processMat);
      cvtColor(processMat, processMat, CV_YCrCb2BGR);
      imshow("Normalized", processMat);
      cvtColor(processMat, processMat, CV_BGR2HSV);// Convert to HSV
      inRange(processMat, Scalar(-5,30,50), Scalar(5,255,250),c1mat);// Filter by color
      imshow("Filter",c1mat);
    }
    if(waitKey(30) >= 0)
    {
      cout << "Exiting" << endl;
      destroyAllWindows();
      break;
    }
  }
  return 0;
}