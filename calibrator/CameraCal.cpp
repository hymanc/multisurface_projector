#include "CameraCal.h"

using namespace std;
using namespace cv;
  CameraCal::CameraCal(VideoCapture *c)
  {
    cap = c;
  }
  
  /**
   * @brief Routine to perform intrinsic camera calibration
   */
  void CameraCal::calibrate(unsigned int n)
  {
    Mat latestImg;
    bool calFlag = false; // Calibration loop flag
    int input;
    bool patternDetect;
    Size patternSize(CORNERS_X, CORNERS_Y);
    vector<Point2f> cornersDetected;
    
    namedWindow("Calibration Preview",1);
    int it;
    for(it = 0; it < n; it++)
    {
      while(waitKey(30) < 0 )
      {
	latestImg = this->getShot();
	imshow("Calibration Preview", latestImg);
	patternDetect = findChessboardCorners(latestImg, patternSize, cornersDetected, CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_FAST_CHECK + CV_CALIB_CB_NORMALIZE_IMAGE);
	if(patternDetect)
	{
	  cornerSubPix(latestImg , cornersDetected , Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
	  pointBuffer.push_back(cornersDetected);
	  break;
	}
	drawChessboardCorners(latestImg, patternSize, Mat(cornersDetected), patternDetect);
      }
      input = waitKey(500);
      if(input == 113) // Check for loop exit command
	break;
    }
  }

  /**
   * @brief Method for querying an image
   * @return Acquired image from camera
   */
  Mat CameraCal::getShot(void)
  {
    Mat outMat;
    cap >> outMat;
    return outMat;
  }
  
  /**
   * @brief Method to assign/reassign the camera capture stream
   * @return Success/failure of assignment
   */
  bool CameraCal::setCaptureSource(VideoCapture *c)
  {
    if(c.isOpened())
    {
      cap = c;
      return true;
    }
    return false;
  }
  
