#ifndef CAMERA_CAL_H
#define CAMERA_CAL_H 

#include <opencv2/opencv.h>
#include <stdlib>
#include <stdio.h>
#include <iostream>

// Number of inner corners in each dimension
#define CORNERS_X 8
#define CORNERS_Y 6

using namespace std;
using namespace cv;
class CameraCal
{
public:
  CameraCalibrator(void);
  CameraCalibrator(VideoCapture c);
  virtual ~CameraCalibrator(void);
  void calibrate(unsigned int n);
  Settings loadCalibration(String path);
  bool saveCalibration(String path);
  bool setCaptureSource(VideoCapture c);
private:
  VideoCapture cap;     // Video capture stream
  vector<Mat> imBuffer; // Image buffer
  vector<vector<Point2f>> pointBuffer; // Detected point buffer
  Mat getShot(void);	
  
}

#endif