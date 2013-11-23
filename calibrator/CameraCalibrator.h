#include <opencv2/opencv.h>
#include <stdlib>
#include <stdio.h>
#include <iostream>

class CameraCalibrator
{
public:
  CameraCalibrator(void);
  CameraCalibrator(VideoCapture c);
  virtual ~CameraCalibrator(void);
  void calibrate();
  Settings loadCalibration(String path);
  bool saveCalibration(String path);
  bool setCaptureSource(VideoCapture c);
private:
  VideoCapture cap;     // Video capture stream
  vector<Mat> imBuffer; // Image buffer
  vector<Point2f> pointBuffer; // Detected point buffer
  Mat getShot(void);	
  Settings settings;
}
