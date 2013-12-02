/*
 * ActiveStereoMap.cpp
 * Active stereo gray code mapping utility
 * Authors: Jakob Hoellerbauer, Cody Hyman, Scott Meldrum
 * December, 2013
 */

#include "ActiveStereoMap.hpp"

using namespace cv;
using namespace std;
/**
 * @brief Active stereo mapper constructor
 * @param c Camera video capture stream
 * @param projSize Projector resolution
 */
ActiveStereoMap::ActiveStereoMap(VideoCapture c, Size projSize)
{
  printf("Initializing stereo mapper\n");
  cap = c;
  Mat sizeMat;
  cap >> sizeMat;
  streamSize = sizeMat.size();
  printf("Camera resolution: %dx%d\n",streamSize.width,streamSize.height);
  sizeMat.release();
  patternSize = projSize; 
  grayPattern = Mat::zeros(patternSize,CV_16UC1);
  grayImg     = Mat::zeros(streamSize,CV_16UC1);
  printf("Creating projector window at %dx%d\n",patternSize.width,patternSize.height);
  namedWindow("Projector",CV_WINDOW_NORMAL);
  moveWindow("Projector",1600,0);
  imshow("Projector",Mat::zeros(patternSize,CV_8UC1));
  namedWindow("Camera",1);
  namedWindow("Process",1);
}

/**
 * @brief Runs a horizontal/vertical gray code mapping routine
 * @param levels Number of gray code levels to use (2^levels) regions
 */
void ActiveStereoMap::runMapping(int levels)
{
  int level;
  Mat patMat  = Mat::zeros(patternSize, CV_16UC1);
  Mat tempPattern;
  cvSetWindowProperty("Projector", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
   waitKey(800);
  for(level = 1; level <= levels; level++)
  {
    printf("Running scan at level %d\n",level);
    tempPattern = ProcGen::getPattern(patternSize,level,VERTICAL, false);
    capturePattern(tempPattern, levels, level);
    
    /*
    waitKey(300);//usleep(3000000);// sleep
    cap >> tempMat;
    cap >> tempMat; 
    cap >> tempMat; // Multiple calls to clear out buffer (empirically...)
    processRawImage(tempMat, procMat, 110, intPow(2,levels-level));
    imshow("Camera",tempMat);
    imshow("Process",procMat);
    printf("Size of grayImg %d x %d\n",grayImg.size().width, grayImg.size().height);
    grayImg += procMat;
    */
    
    tempPattern = ProcGen::getPattern(patternSize,level,HORIZONTAL,false);
    imshow("Projector", tempPattern);
    capturePattern(tempPattern, levels, level);
    
  }
}

/**
 * 
 */
void ActiveStereoMap::capturePattern(Mat tempPattern, int levels, int level)
{
  Mat patMat;
  imshow("Projector", tempPattern);
  tempPattern.convertTo(patMat, CV_16UC1, 1, 0);
  grayPattern += intPow(2,levels-level) * (patMat/255);  
  
  Mat tempMat;
  Mat procMat = Mat::zeros(streamSize,CV_16UC1);
  waitKey(300);//usleep(3000000);// sleep
  cap >> tempMat;
  cap >> tempMat; 
  cap >> tempMat; // Multiple calls to clear out buffer (empirically...)
  processRawImage(tempMat, procMat, 110, intPow(2,levels-level));
  imshow("Camera",tempMat);
  //imshow("Process",procMat);
  //printf("Size of grayImg %d x %d\n",grayImg.size().width, grayImg.size().height);
  grayImg += procMat;
}

/**
 * @brief Computes the disparity between the images given a calibrated fundamental matrix
 * @param f Fundamental matrix describing transform from camera->projector frame
 * @return Disparity map image from the projector perspective
 */
Mat ActiveStereoMap::computeDisparity(Mat f)
{
  Mat retMat;
  // TODO: everything in this function
  // Rectify-Transform camera F*grayMap 
  // Gray-filter camera image
  // Find distance to matching patch along epipolar line
  return retMat;
}

/**
 * @brief Accessor for the camera frame raw gray map
 * @return Camera composite gray map
 */
Mat ActiveStereoMap::getGraymap(void)
{
    return grayImg;
}

/**
 * @brief Accessor for the projector frame gray map
 * @return Projector composite gray map
 */
Mat ActiveStereoMap::getGrayComposite(void)
{
    return grayPattern;
}

/**
 * @brief Does color conversion and thresholding to raw gray frames
 * @param rawImg The raw input image
 * @param destImg Destination image container
 * @param thresh Image threshold 0-255
 */
void ActiveStereoMap::processRawImage(Mat rawImg, Mat destImg, int thresh, int factor)
{
  Mat temp;
  cvtColor(rawImg, temp, CV_RGB2GRAY);
  medianBlur(temp, temp, 5);
  threshold(temp, temp, thresh, 1 ,THRESH_BINARY);
  temp.convertTo(destImg, CV_16UC1, 1, 0);
  destImg = factor * destImg;
  //printf("Size of destImg %d x %d\n",destImg.size().width, destImg.size().height);
}

/**
 * @brief Simple integer power functoin
 * @param base Base integer value
 * @param pow  Integer power 
 * @return Computes base^power
 */
int ActiveStereoMap::intPow(int base, unsigned int pow)
{
  if(pow == 0)
    return 1;
  int acc = base;
  int i;
  for(i=1;i<pow;i++)
  {
    acc *= base;
  }
  return acc;
}