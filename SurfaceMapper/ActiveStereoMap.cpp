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
  namedWindow("Projector",CV_WINDOW_NORMAL);
  //namedWindow("Elephant",1);
  cal = new ActiveCal();
  cal->calibratePassive();
  printf("Initializing stereo mapper\n");
  cap = c;
  Mat sizeMat;
  cap >> sizeMat;
  streamSize = sizeMat.size();
  printf("Camera resolution: %dx%d\n",streamSize.width,streamSize.height);
  sizeMat.release();
  waitKey(1000);
  patternSize = projSize; 
  grayPattern = Mat::zeros(patternSize,CV_16UC1);
  grayProjH  = Mat::zeros(patternSize,CV_16UC1);
  grayProjV  = Mat::zeros(patternSize,CV_16UC1);
  grayImg = Mat::zeros(streamSize,CV_16UC1);
  grayH = Mat::zeros(patternSize,CV_16UC1);
  grayV = Mat::zeros(patternSize,CV_16UC1);
  printf("Creating projector window at %dx%d\n",patternSize.width,patternSize.height);
  
  moveWindow("Projector",1600,0);
  cvSetWindowProperty("Projector", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  Mat filler = Mat::zeros(patternSize,CV_8UC1);
  imshow("Projector",filler);
  namedWindow("Camera",1);
  namedWindow("Process",1);
  
   int offset_x = ROI_X_OFFSET; //100;
   int offset_y = ROI_Y_OFFSET; //100;
   int xsize = ROI_WIDTH; //800;  // ROI size
   int ysize = ROI_HEIGHT; //600;
  
   roi = Rect(offset_x,offset_y,xsize,ysize);
}

/**
 * @brief Runs a horizontal/vertical gray code mapping routine
 * @param levels Number of gray code levels to use (2^levels) regions
 */
void ActiveStereoMap::runMapping(int levels, bool bidir)
{
  int level;
  numLevels = levels;
  stripeSize = ProcGen::getMinimumStripeWidth(patternSize, levels);
  Mat patMat  = Mat::zeros(patternSize, CV_16UC1);
  Mat tempPattern;
  Mat tempMat;
  
  grayPattern = Mat::zeros(patternSize,CV_16UC1);
  grayProjH  = Mat::zeros(patternSize,CV_16UC1);
  grayProjV  = Mat::zeros(patternSize,CV_16UC1);
  grayImg = Mat::zeros(streamSize,CV_16UC1);
  grayH = Mat::zeros(patternSize,CV_16UC1);
  grayV = Mat::zeros(patternSize,CV_16UC1);
  
  waitKey(1200);
  cap >> tempMat;
  //imshow("Camera",tempMat);
  for(level = 1; level <= levels; level++)
  {
    printf("Running scan at level %d\n",level);
    tempPattern = ProcGen::getPattern(patternSize,level,VERTICAL, false);
    capturePattern(tempPattern, tempMat, levels, level, false);
    
    if(bidir) // Bidirectional mapping
    {
      tempPattern = ProcGen::getPattern(patternSize,level,HORIZONTAL,false);
      capturePattern(tempPattern, tempMat, levels, level, true);
    }
  }
  generateGrayLuts(); // Generate gray->binary lookup
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
  Mat patMat, invPatMat; // 16bit patterns
  Mat invTempPattern;	 // Inverted projector pattern (8bit)
  Mat cam, invCam;       // Normal and inverted camera captures
  
  // Get normal pattern and do a few other things in the meantime
  imshow("Projector", tempPattern);
  bitwise_not(tempPattern, invTempPattern); // Generate inverted pattern
  Mat procMat = Mat::zeros(Size(PROJECTOR_W,PROJECTOR_H),CV_16UC1);
  waitKey(700);//usleep(3000000);// sleep
  int trash;
  for(trash=0;trash<3;trash++) // Clear out frames
    cap >> tempMat;
  cap >> tempMat;
  cam = tempMat(roi).clone();
  
  // Get Inverted Pattern
  imshow("Projector", invTempPattern);
  waitKey(700);
  for(trash=0;trash<3;trash++)
    cap >> tempMat;
  cap >> tempMat;
  invCam = tempMat(roi).clone();
  
  tempMat = cam-invCam; // Extract differential signal
  processRawImage(tempMat, procMat, 5, intPow(2,levels-level));
  //imshow("Camera",tempMat);
  waitKey(50);
  imshow("Process",65535*procMat);
  tempPattern.convertTo(patMat, CV_16UC1, 1, 0);
  invTempPattern.convertTo(invPatMat, CV_16UC1, 1, 0);
  
  // Resize to 640 x 480
  resize(patMat,patMat,Size(PROJECTOR_W,PROJECTOR_H),0,0,INTER_NEAREST);
  
  // Store out to rolling buffer
  int multiplier = intPow(2,levels-level);
  if(horizontalFlag) // Check for buffer to put image into
  {
    grayProjH += multiplier*(patMat/255);
    grayH += procMat; 
  }
  else // Assume vertical
  {
    grayProjV += multiplier*(patMat/255);
    grayV += procMat;
  }
}

/**
 * @brief Computes the disparity between the images given a calibrated fundamental matrix
 * @return Disparity map image from the projector perspective
 */
Mat ActiveStereoMap::computeDisparity(void)
{
  
  Mat rectCam, rectProj;
  Size targetSize(640,480);
  Mat dispMap = Mat::zeros(targetSize,CV_32F);
  cal->rectifyImages(grayV,grayProjV,&rectCam,&rectProj);
  namedWindow("Rectified Projector",1);
  namedWindow("Rectified Camera",1);
  Mat showCam, showProj, showDisp;

  int x,y;
  uint grayVal;
  float disp;
  for(y=0;y<rectProj.size().height;y++)
  {
    for(x=0;x<rectProj.size().width;x++)
    {
      grayVal = rectProj.at<short>(y,x);
      if(grayVal != 0)
      {
	disp = expandingSearch(rectCam,x,y,grayVal,60);
	if(disp !=-32768)
	{
	  //dispMap(Range(y,y),Range(x,x)) = disp;
	  dispMap.at<float>(y,x) = disp;
	  printf("%f\n",dispMap.at<float>(y,x));
	}
      }
    }
  }
  // Search for value
  rectProj = rectProj / 2;
  rectCam = rectCam / 2;
  rectCam.convertTo(showCam, CV_8UC1);
  rectProj.convertTo(showProj,CV_8UC1);
  dispMap.convertTo(showDisp, CV_8UC1);

  imshow("Rectified Projector", showProj);
  imshow("Rectified Camera",showCam);
  imshow("Disparity",showDisp);
  waitKey(0);
  return dispMap;
}

/**
 * 
 */
float ActiveStereoMap::expandingSearch(Mat searchImg, uint xloc, uint yloc, uint grayVal, uint cutoff)
{
  short xp = 0;
  while(xp <= cutoff)
  {
    if(searchImg.at<short>(yloc,xloc+xp) == grayVal)
    {
      printf("Found match at %d\n",xp);
      return (float) xp;
    }
    else if(searchImg.at<short>(yloc,xloc-xp) == grayVal)
    {
      printf("Found match at -%d\n",xp);
      return (float)(-1*xp);
    }
    xp++;
  } 
  return (float)(-32768);
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
 * @brief Accessor for projector pattern resolution
 * @return Projector pattern Cv::Size
 */
Size ActiveStereoMap::getPatternSize(void)
{
  return patternSize;
}

/**
 * @brief Does color conversion and thresholding to raw gray frames
 * @param rawImg The raw input image
 * @param destImg Destination image container
 * @param thresh Image threshold 0-255
 */
void ActiveStereoMap::processRawImage(Mat rawImg, Mat destImg, int thresh, int factor)
{
  Mat temp, tempResize;
  tempResize = Mat::zeros(Size(PROJECTOR_W,PROJECTOR_H),CV_8UC1);
  //Scalar scale = mean(rawImg)/8;
  cvtColor(rawImg, temp, CV_RGB2GRAY);
  medianBlur(temp, temp, 3);
  threshold(temp, temp, thresh, 1 ,THRESH_BINARY);
  resize(temp,tempResize,Size(PROJECTOR_W,PROJECTOR_H),0,0,INTER_NEAREST);
  tempResize.convertTo(destImg, CV_16UC1, 1, 0);
  destImg = factor * destImg;
  //printf("Size of tempImg %d x %d\n",tempResize.size().width, tempResize.size().height);
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
  * @brief Generates Gray->Row/Col lookup tables
  * @param vImg Columnwise gray code pattern
  * @param hImg Rowwise gray code pattern
  */
 void ActiveStereoMap::generateGrayLuts(void)
 {
   int maxGray = intPow(2,numLevels);
   hLUT = get_row_LUT(grayProjH, maxGray);
   vLUT = get_column_LUT(grayProjV, maxGray);
 }
 
/**
 * @brief Displays a solid max value image on the projector
 */
 void ActiveStereoMap::showWhite(uchar brightness)
 {
   Mat whiteMat = brightness*Mat::ones(patternSize,CV_8UC1);
   imshow("Projector",whiteMat);
   printf("Moving window\n");
   moveWindow("Projector",1600,0);
   printf("Window moved\n");
   cvSetWindowProperty("Projector", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
 }

 /**
  */
void ActiveStereoMap::printGrayHSize(void)
{
  printf("grayH actual size is %dx%d\n",grayH.size().width,grayH.size().height);
  uint grayVal = grayH.at<short>(220,220);
  printf("Gray Val: %d\n",grayVal);
}