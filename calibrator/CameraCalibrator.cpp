#include "CameraCalibrator.h"


using namespace CameraCal;
using namespace cv;
  CameraCalibrator(VideoCapture c)
  {
    cap = c;
  }
  
  /**
   * @brief Routine to perform intrinsic camera calibration
   */
  void calibrate(void)
  {
    Mat latestImg;
    bool calFlag = false; // Calibration loop flag
    int input;
    while(!calFlag)
    {
      latestImg = getShot();
      //imBuffer.add(latestImg);
      findChessboardCorners(latestImg, settings.boardSize, pointBuffer, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
    
      // Wait for user input
      input = waitKey(0);
      if(input == 113); // Wait for input
      {
	break;
      }
    }
  }
  
  Settings loadCalibration(String path)
  {
    return null;
  }
  
  bool saveCalibration(String path)
  {
    return false;
  }

  /**
   * @brief Method for querying an image
   * @return Acquired image from camera
   */
  Mat getShot(void)
  {
    Mat outMat;
    cap >> outMat;
    return outMat;
  }
  
  /**
   * @brief Method to assign/reassign the camera capture stream
   * @return Success/failure of assignment
   */
  bool setCaptureSource(VideoCapture c)
  {
    if(c != null)
    {
      cap = c;
      return true;
    }
    return false;
  }
  
  
}
