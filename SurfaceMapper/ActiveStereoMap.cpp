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
  //namedWindow("Elephant",CV_WINDOW_NORMAL);
  //namedWindow("Elephant",1);
  printf("Initializing stereo mapper\n");
  cap = c;
  Mat sizeMat;
  cap >> sizeMat;
  streamSize = sizeMat.size();
  printf("Camera resolution: %dx%d\n",streamSize.width,streamSize.height);
  sizeMat.release();
  
  patternSize = projSize; 
  grayPattern = Mat::zeros(patternSize,CV_16UC1);
  grayProjH  = Mat::zeros(patternSize,CV_16UC1);
  grayProjV  = Mat::zeros(patternSize,CV_16UC1);
  grayImg = Mat::zeros(streamSize,CV_16UC1);
  grayH = Mat::zeros(streamSize,CV_16UC1);
  grayV = Mat::zeros(streamSize,CV_16UC1);
  printf("Creating projector window at %dx%d\n",patternSize.width,patternSize.height);
  moveWindow("Projector",1600,0);
  cvSetWindowProperty("Projector", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  Mat filler = Mat::zeros(patternSize,CV_8UC1);
  imshow("Projector",filler);
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
  stripeSize = ProcGen::getMinimumStripeWidth(patternSize, levels);
  Mat patMat  = Mat::zeros(patternSize, CV_16UC1);
  Mat tempPattern;
  Mat tempMat;
  waitKey(1200);
  cap >> tempMat;
  imshow("Camera",tempMat);
  for(level = 1; level <= levels; level++)
  {
    printf("Running scan at level %d\n",level);
    tempPattern = ProcGen::getPattern(patternSize,level,VERTICAL, false);
    capturePattern(tempPattern, tempMat, levels, level, false);
    
    tempPattern = ProcGen::getPattern(patternSize,level,HORIZONTAL,false);
    //imshow("Projector", tempPattern);
    capturePattern(tempPattern, tempMat, levels, level, true);
  }
}

/**
 * @brief Projects and captures the given pattern
 * @param tempPattern The current pattern to project
 * @param tempMat A temporary buffer for captured images
 * @param levels The total number of gray code levels used
 * @param level The current gray code level
 * @param horizontalFlag Flag that should be true if the pattern is horizontal
 */
void ActiveStereoMap::capturePattern(Mat tempPattern, Mat tempMat, int levels, int level, bool horizontalFlag)
{
  Mat patMat;
  imshow("Projector", tempPattern);
  int multiplier;
  
  tempPattern.convertTo(patMat, CV_16UC1, 1, 0);
  //grayPattern += multiplier * intPow(2,levels-level) * (patMat/255);  

  //Mat tempMat;
  Mat procMat = Mat::zeros(streamSize,CV_16UC1);
  waitKey(600);//usleep(3000000);// sleep
  cap >> tempMat;
  cap >> tempMat; 
  cap >> tempMat; // Multiple calls to clear out buffer (empirically...)
  cap >> tempMat;
  processRawImage(tempMat, procMat, 100, intPow(2,levels-level));
  imshow("Camera",tempMat);
  //imshow("Process",procMat);
  //printf("Size of grayImg %d x %d\n",grayImg.size().width, grayImg.size().height);
  if(horizontalFlag)
  {
    //grayImg += multiplier * procMat;
    grayProjH += intPow(2,levels-level)*(patMat/255);
    grayH += procMat;
  }
  else
  {
    grayProjV += intPow(2,levels-level)*(patMat/255);
    grayV += procMat;
  }
}

/**
 * @brief Computes the disparity between the images given a calibrated fundamental matrix
 * @return Disparity map image from the projector perspective
 */
Mat ActiveStereoMap::computeDisparity(Mat dCam, Mat dProj, Mat R, Mat T)
{
  Mat retMat;
  // TODO: everything in this function
  //stereoRectify(
  // Gray-filter camera image
  // Find distance to matching patch along epipolar line
  return retMat;
}

/**
 * @brief Accessor for horizontal gray camera image
 * @brief Horizontal gray pattern in camera frame
 */
Mat ActiveStereoMap::getGrayH(void)
{
    return grayH;
}

/**
 * @brief Accessor for vertical gray camera image
 * @return Vertical gray pattern in camera frame
 */
Mat ActiveStereoMap::getGrayV(void)
{
    return grayV;
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
 * @brief Accessor for projector horizontal gray pattern
 * @return Horizontal gray pattern in projector frame
 */
Mat ActiveStereoMap::getGrayProjH(void)
{
  return grayProjH;
}

/**
 * @brief Accessor for projector vertical gray pattern
 * @return Vertical gray pattern in projector frame
 */
Mat ActiveStereoMap::getGrayProjV(void)
{
  return grayProjV;
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
  //Scalar scale = mean(rawImg)/8;
  cvtColor(rawImg, temp, CV_RGB2GRAY);
  medianBlur(temp, temp, 5);
  threshold(temp, temp, thresh, 1 ,THRESH_BINARY);
  temp.convertTo(destImg, CV_16UC1, 1, 0);
  destImg = factor * destImg;
  //printf("Size of destImg %d x %d\n",destImg.size().width, destImg.size().height);
}

/**
 * @brief Simple integer power function
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

/**
 * @brief Simple gray value filter
 * @param src Source image
 * @param graylvl Desired gray level to search for
 * @return Filtered image with desired gray values at maximum value, all other pixels are 0
 */
Mat ActiveStereoMap::grayFilter(Mat src, int graylvl)
{
  Mat outMat;
  inRange(src,Scalar(graylvl),Scalar(graylvl), outMat);
  return outMat;
}

/**
 * @brief Gray code to binary converter
 * @param graylvl Gray code representation to convert from
 * @return Binary representation of gray code value
 */
unsigned int ActiveStereoMap::grayToBinary(unsigned int graylvl)
{
  unsigned int mask;
  for (mask = graylvl >> 1; mask != 0; mask = mask >> 1)
  {
      graylvl = graylvl ^ mask;
  }
  return graylvl;
}

/**
 * 
 */
Mat ActiveStereoMap::getWindow(int x, int y, int size)
{
  Mat window;
  // Get ROI window centered around pixel 
  return window;
}

/**
 * 
 */
 Vec2b ActiveStereoMap::findGrayCoordinates(int graylvlH, int graylvlV, int nlvls, Size mapSize)
 {
   Vec2b pcoord;
   uint px, py;

   return pcoord;
 }
 
/**
 * @brief Displays a solid max value image on the projector
 */
 void ActiveStereoMap::showWhite(void)
 {
   Mat whiteMat = 255*Mat::ones(patternSize,CV_8UC1);
   imshow("Projector",whiteMat);
   moveWindow("Projector",1600,0);
   cvSetWindowProperty("Projector", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
 }