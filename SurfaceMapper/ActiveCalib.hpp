#ifndef ACTIVE_CALIB
#define ACTIVE_CALIB

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;
  
typedef struct
{
  Mat R;
  Mat T;
  Mat F;
  Mat DistProj;
  Mat DistCam;
  Mat MProj;
  Mat MCam;
} cal_set;

// Stereo rectification (uncal) data
typedef struct
{
  Mat PCam;
  Mat PProj;
  Mat RCam;
  Mat RProj;
  Mat HCam;
  Mat HProj;
  Mat MCam;
  Mat MProj;
  Mat DistCam;
  Mat DistProj;
  Mat F;
  Mat camMap[2];
  Mat projMap[2];
 
} stereo_rect;

class ActiveCal
{
public:
  bool calibrate(VideoCapture c, Size cboardSize, float sqWidth, float sqHeight, int lvls, int ncals);
  bool calibratePassive(void);
  void calMapping(void);
  //stereo_rect getStereoRectification(vector<vector<Vec2f> > camSet, vector<vector<Vec2f> > projSet);
  bool saveCalPointsToFile(vector<vector<Vec3f> > objSet, vector<vector<Vec2f> > camSet, vector<vector<Vec2f> > projSet);
  bool readCalPointsFromFile(vector<vector<Vec3f> > *objSet, vector<vector<Vec2f> > *camSet, vector<vector<Vec2f> > *projSet);
  cal_set * getCals(void);
  vector<Mat> getRectificationTransforms(void);
  static string floatToString(float f);
  static float stringToFloat(string s);
  
  void rectifyImages(Mat im1, Mat im2, Mat *rectIm1, Mat *rectIm2);
  //bool readCalFromFile(bool setGlobalCals, cal_set * loadCals); 
  //bool writeCalToFile(cal_set * saveCals);
private:
  vector<Vec3f> objCorners;
  
  vector< vector<Vec3f> > objCornersGroup;
  vector< vector<Vec2f> > camCorners;
  vector< vector<Vec2f> > projCorners;
  cal_set *cals;
  
  bool calibrated;
  
  stereo_rect RC;

  
  vector<Vec2f> backprojectPoints(Mat grayMapCamH, Mat grayMapCamV, Mat grayMapProjH, Mat grayMapProjV, int cellSize, vector<Vec2f> camCorners, vector<Vec2f> projCorners);
  void printPoints(vector<Vec2f> points); 
  void generateObjectCorners(Size cboardSize, float sqWidth, float sqHeight);
};

#endif