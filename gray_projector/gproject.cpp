#include <opencv2/opencv.hpp>
#include "GrayProjector.hpp"
#include <stdio.h>
#include <stdlib.h>

#define LEVELS 10
using namespace std;
using namespace cv;
int main(int argc, char **argv)
{
  int imCount;
  int cam = atoi(argv[1]);
  VideoCapture cap(cam);
  if(!cap.isOpened())
    return -1;
  namedWindow("GrayPatterns",CV_WINDOW_NORMAL);
  setWindowProperty("GrayPatterns", CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);
  GrayProjector *gp = new GrayProjector();
  Mat currentView;
  Mat currentOutput;
  for(imCount = 1; imCount <= LEVELS; imCount++)
  {
    currentOutput = gp->getGrayPattern(imCount,false);
    imshow("GrayPatterns",currentOutput);
    waitKey(100);
    cap >> currentView;
  }
  
  waitKey(0);
  return 0;
}