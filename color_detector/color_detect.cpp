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
  Mat bw;
  vector<Mat> channels;
  namedWindow("Input",1);
  namedWindow("Normalized",1);
  namedWindow("Filter",1);
  namedWindow("BW",1);
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
      cvtColor(processMat,bw,CV_BGR2GRAY);
      medianBlur(processMat, processMat, 5); // Median blur to reduce camera noise
      //cvtColor(processMat, processMat, CV_BGR2YCrCb);
      //split(processMat, channels);
      //equalizeHist(channels[0], channels[0]);
      //merge(channels, processMat);
      //cvtColor(processMat, processMat, CV_YCrCb2BGR);
      Scharr(bw, bw, CV_8U,1,0,1,5, BORDER_DEFAULT);
      medianBlur(bw, bw, 3);
      inRange(bw, Scalar(50), Scalar(255), bw);
      imshow("Normalized", processMat);
      imshow("BW",bw);
      cvtColor(processMat, processMat, CV_BGR2HSV);// Convert to HSV
      inRange(processMat, Scalar(170,0,40), Scalar(172,255,200),c1mat);// Filter by color
      GaussianBlur(c1mat,c1mat,Size(5,5),0,0);
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