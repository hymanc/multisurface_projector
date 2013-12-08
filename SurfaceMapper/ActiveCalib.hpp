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
  vector<Vec2f> backprojectPoints(Mat grayMapCam, Mat grayMapProj, vector<Vec2f> camCorners);
  Vec2f findGrayCoordinate(int graylvl, int nlevels, Size mapSize);
  void printPoints(vector<Vec2f> points); 
};

#endif