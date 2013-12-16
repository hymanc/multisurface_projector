#ifndef ACTIVE_CALIB
#define ACTIVE_CALIB

#include "ActiveStereoMap.hpp"
#include <vector>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;
  
typedef struct
{
  Mat R;
  Mat T;
  Mat E;
  Mat F;
  Mat DistProj;
  Mat DistCam;
  Mat MProj;
  Mat MCam;
} cal_set;

class ActiveCal
{
public:
  bool calibrate(cal_set * cals, VideoCapture c, Size cboardSize, float sqWidth, float sqHeight, int lvls, int ncals);
  void calMapping(void);
  bool saveCalPointsToFile(vector<vector<Vec3f> > objSet, vector<vector<Vec2f> > camSet, vector<vector<Vec2f> > projSet);
  bool readCalPointsFromFile(vector<vector<Vec3f> > objSet, vector<vector<Vec2f> > camSet, vector<vector<Vec2f> > projSet);
  cal_set * getCals(void);
  static string floatToString(float f);
  static float stringToFloat(string s);
private:
  vector<Vec3f> objCorners;
  cal_set *cals;
  
  vector<Vec2f> backprojectPoints(Mat grayMapCamH, Mat grayMapCamV, Mat grayMapProjH, Mat grayMapProjV, int cellSize, vector<Vec2f> camCorners, vector<Vec2f> projCorners);
  void printPoints(vector<Vec2f> points); 
  void generateObjectCorners(Size cboardSize, float sqWidth, float sqHeight);
  bool readCalFromFile(bool setGlobalCals, cal_set * loadCals); 
  bool writeCalToFile(cal_set * saveCals);
};

#endif