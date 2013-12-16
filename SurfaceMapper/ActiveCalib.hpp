#ifndef ACTIVE_CALIB
#define ACTIVE_CALIB

#include "ActiveStereoMap.hpp"
#include <vector>

using namespace cv;
using namespace std;
  
typedef struct
{
  Mat R;
  Mat T;
  Mat D1;
  Mat D2;
  Mat M1;
  Mat M2;
} cal_set;

class ActiveCal
{
public:
  bool calibrate(cal_set * cals, VideoCapture c, Size cboardSize, float sqWidth, float sqHeight, int lvls, int ncals);
private:
  vector<Vec3f> objCorners;
  cal_set cals;
  
  vector<Vec2f> backprojectPoints(Mat grayMapCamH, Mat grayMapCamV, Mat grayMapProjH, Mat grayMapProjV, int cellSize, vector<Vec2f> camCorners, vector<Vec2f> projCorners);
  void printPoints(vector<Vec2f> points); 
  void generateObjectCorners(Size cboardSize, float sqWidth, float sqHeight);
};

#endif