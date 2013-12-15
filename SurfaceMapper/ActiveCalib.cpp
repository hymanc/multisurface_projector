#include "ActiveCalib.hpp"
#include <opencv2/opencv.hpp>

/**
 * @brief Determines a stereo calibration for an active pair using gray coding and a checkerboard
 * @param cals Return calibration pointer
 * @param c Video capture stream to use
 * @param cboardSize Dimension of checkerboard in terms of inner corners
 * @param sqWidth Metric width of checkerboard squares
 * @param sqHeight Metric height of checkerboard squares
 * @param lvls Number of gray levels to use for each calibration shot
 * @param ncals Number of calibration shots to take 
 * @return Success of running the calibration routine
 */
bool ActiveCal::calibrate(cal_set * cals, VideoCapture c, Size cboardSize, float sqWidth, float sqHeight, int lvls, int ncals)
{
  namedWindow("Preview",1);
  Mat cam, graycam;
  Mat grayMapCamH, grayMapCamV, grayMapProjH, grayMapProjV;
  c >> cam;
  cvtColor(cam, graycam, CV_RGB2GRAY);
  vector<Vec2f> camCorners, projCorners;
  ActiveStereoMap *map = new ActiveStereoMap(c,Size(PROJECTOR_W,PROJECTOR_H));
  int i;
  for(i = 0; i<ncals; i++)
  {
    map->showWhite();
    bool foundCheckersFlag = false;
    // Wait for 
    while(waitKey(30) <= 0 || !foundCheckersFlag)
    {
      c >> cam;
      c >> cam;
      c >> cam;
      c >> cam; // Capture image
      cvtColor(cam,graycam, CV_RGB2GRAY);
      foundCheckersFlag = findChessboardCorners(graycam, cboardSize, camCorners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
      if(foundCheckersFlag)
      {
	printf("Found checkerboard\n");
	cornerSubPix(graycam, camCorners, Size(11,11), Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,30,0.1));
	//printPoints(camCorners);
      }
      // Draw over image
      drawChessboardCorners(cam, cboardSize, Mat(camCorners),foundCheckersFlag); 
      imshow("Preview",cam);
      if(waitKey(1) >= 0)
	return false;
    }// Wait for keypress
    map->runMapping(lvls);// Run through pattern
    grayMapCamH = map->getGrayH();
    grayMapCamV = map->getGrayV();
    grayMapProjH = map->getGrayProjH();
    grayMapProjV = map->getGrayProjV();
    projCorners = backprojectPoints(grayMapCamH, grayMapCamV, grayMapProjH, grayMapProjV, camCorners);
    // Iterate over chesskerboard points
    // Correlate chesskerboard corners to projector from graymap locations
    // Store out correspondence pair
  } 
  return true;
}

/**
 * @brief Finds the corresponding set of points in the projector frame
 * @param grayMapCamH Horizontal gray pattern camera image
 * @param grayMapCamV Vertical gray pattern camera image
 * @param grayMapProjH Horizontal gray pattern projector image
 * @param grayMapProjV Vertical gray pattern projector image
 * @param camCorners Detected checkerboard corners in camera image
 * @return Checkerboard corner locations in the projector frame
 */
vector<Vec2f> ActiveCal::backprojectPoints(Mat grayMapCamH, Mat grayMapCamV, Mat grayMapProjH, Mat grayMapProjV, vector<Vec2f> camCorners)
{
  vector<Vec2f> projCorners;
  int len = camCorners.size();
  int it;
  int imgX, imgY;
  uint projX, projY;
  Scalar currentGrayH, currentGrayV;
  Vec2f pixCoord;
  printf("Backprojecting Corners\n");
  for(it = 0; it < camCorners.size(); it++)
  {
    pixCoord = camCorners.at(it);
    imgY = (int) (pixCoord[0] + 0.5);
    imgX = (int) (pixCoord[1] + 0.5);
    currentGrayH = grayMapCamH.at<float>(pixCoord[0],pixCoord[1]);// Find gray value at corner location
    currentGrayV = grayMapCamV.at<float>(pixCoord[0],pixCoord[1]);
    projX = ActiveStereoMap::grayToBinary(currentGrayV[0]); // Convert to index in projector frame
    projY = ActiveStereoMap::grayToBinary(currentGrayH[0]);
    printf("Gray Level is %d,%d at %d,%d\n",(int)currentGrayH[0],(int)currentGrayV[0],imgX,imgY);
    
    //ActiveCal::grayToBinary(currentGray);
    // Find corresponding gray value in projector map and save point 
  }
  return projCorners;
}

/**
 * @brief Utility function to list the current calibration point locations in the camera frame
 * @param points Vector of points to print
 */
void ActiveCal::printPoints(vector<Vec2f> points)
{
  int i;
  Vec2f pt;
  for(i = 0; i < points.size(); i++)
  {
    pt = points.at(i);
    printf("Point at %f,%f\n",pt[0],pt[1]);
  }
}