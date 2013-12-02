#include "ActiveStereoMap.hpp"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;
int main(int argc, char ** argv)
{
  if(argc < 3)
  {
    printf("Invalid number of input arguments\n");
    return -1;
  }
  VideoCapture cap(atoi(argv[1]));
  if(!cap.isOpened())
  {
    printf("Camera capture failed to initialize, exiting\n");
  }
  else
  {
      cap.set(CV_CAP_PROP_FRAME_WIDTH, 960);
      cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
      //cap.set(CV_CAP_PROP_EXPOSURE, 150);
      //cap.set(CV_CAP_PROP_BRIGHTNESS, 136);
      //cap.set(CV_CAP_PROP_CONTRAST, 33);
      //cap.set(CV_CAP_PROP_GAIN, 200);
      //cap.set(CV_CAP_PROP_EXPOSURE, 0.5882);
      cap.set(CV_CAP_PROP_BRIGHTNESS, 0.5333);
      cap.set(CV_CAP_PROP_CONTRAST, 0.1294);
      //cap.set(CV_CAP_PROP_GAIN, 0.7843);
      cap.set(CV_CAP_PROP_FPS, 15.0);
  }
  
  ActiveStereoMap *smapper = new ActiveStereoMap(cap, Size(848,480));
  smapper->runMapping(atoi(argv[2]));
 
  Mat gray = smapper->getGraymap()/2;
  Mat grayPat = smapper->getGrayComposite()/2;
  gray.convertTo(gray,CV_8UC1,1,0);
  grayPat.convertTo(grayPat, CV_8UC1, 1, 0);

  namedWindow("Graymap",1);
  imshow("Graymap",gray);
  namedWindow("GrayPattern",1);
  imshow("GrayPattern",grayPat);
  waitKey(0);
  return 0;
}