#include "ActiveCalib.hpp"
#include "ActiveStereoMap.hpp"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;
int main(int argc, char ** argv)
{
  if(argc < 4)
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
    //960x720
      cap.set(CV_CAP_PROP_FRAME_WIDTH, 1600);
      cap.set(CV_CAP_PROP_FRAME_HEIGHT,1200);
      
      double xrescheck = cap.get(CV_CAP_PROP_FRAME_WIDTH);
      double yrescheck = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
      cap.set(CV_CAP_PROP_GAIN, 0.6);
      //cap.set(CV_CAP_PROP_EXPOSURE, 0.5882);
      cap.set(CV_CAP_PROP_BRIGHTNESS, 0.5);
      cap.set(CV_CAP_PROP_CONTRAST, 0.2);
      //cap.set(CV_CAP_PROP_GAIN, 0.7843);
      cap.set(CV_CAP_PROP_FPS, 5);
      
      double fps = cap.get(CV_CAP_PROP_FPS);
      printf("Camera resolution set to %dx%d\n",(int)xrescheck,(int)yrescheck);
      printf("%d FPS\n",(int)fps);
  }
  
  ActiveCal *cal = new ActiveCal();
  int calRuns = atoi(argv[3]);
  
  if(calRuns > 0)
  {
    cal->calibrate(cap, Size(8,6),14,14,atoi(argv[2]),calRuns);
  }
  else
  {
    ActiveStereoMap *smapper = new ActiveStereoMap(cap, Size(PROJECTOR_W,PROJECTOR_H));
    smapper->runMapping(atoi(argv[2]),false);
    smapper->computeDisparity();
  }
  //smapper->runMapping(atoi(argv[2]));
  /*
  Mat grayhorz = smapper->getGrayH();
  Mat grayvert = smapper->getGrayV();
  Mat grayPatH = smapper->getGrayProjH()/2;
  Mat grayPatV = smapper->getGrayProjV()/2;
  grayvert.convertTo(grayvert,CV_8UC1,1,0);
  grayPatV.convertTo(grayPatV, CV_8UC1, 1, 0);
  namedWindow("Graymap",1);
  imshow("Graymap",grayvert);
  namedWindow("GrayPattern",1);
  imshow("GrayPattern",grayPatV);*/
  waitKey(0);
  
  return 0;
}