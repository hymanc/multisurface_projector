#include "ActiveCalib.hpp"
#include <opencv2/opencv.hpp>

/**
 * 
 */
bool ActiveCal::calibrate(cal_set * cals, VideoCapture c, Size cboardSize, float sqWidth, float sqHeight, int lvls, int ncals)
{
  namedWindow("Preview",1);
  Mat cam, graycam;
  Mat grayMapCam, grayMapProj;
  c >> cam;
  cvtColor(cam, graycam, CV_RGB2GRAY);
  vector<Vec2f> camCorners, projCorners;
  ActiveStereoMap *map = new ActiveStereoMap(c,Size(PROJECTOR_W,PROJECTOR_H));
  int i;
  for(i = 0; i<ncals; i++)
  {
    bool foundCheckersFlag = false;
    // Wait for 
    while(waitKey(30) <= 0 || !foundCheckersFlag)
    {
      c >> cam; // Capture image
      cvtColor(cam,graycam, CV_RGB2GRAY);
      foundCheckersFlag = findChessboardCorners(graycam, cboardSize, camCorners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
      if(foundCheckersFlag)
      {
	printf("Found checkerboard\n");
	cornerSubPix(graycam, camCorners, Size(11,11), Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,30,0.1));
	printPoints(camCorners);
      }
      // Draw over image
      drawChessboardCorners(cam, cboardSize, Mat(camCorners),foundCheckersFlag); 
      imshow("Preview",cam);
      if(waitKey(1) >= 0)
	return false;
    }// Wait for keypress
    map->runMapping(lvls);// Run through pattern
    grayMapCam = map->getGraymap();
    grayMapProj = map->getGrayComposite();
    projCorners = backprojectPoints(grayMapCam, grayMapProj, camCorners);
    // Iterate over chesskerboard points
    // Correlate chesskerboard corners to projector from graymap locations
    // Store out correspondence pair
  } 
}

/**
 * @brief Finds the corresponding set of points in the projector frame
 * 
 */
vector<Vec2f> ActiveCal::backprojectPoints(Mat grayMapCam, Mat grayMapProj, vector<Vec2f> camCorners)
{
  vector<Vec2f> projCorners;
  int len = camCorners.size();
  int it;
  Scalar currentGray;
  Vec2b pixCoord;
  for(it = 0; it < camCorners.size(); it++)
  {
    camCorners.ad(it).convertTo(pixCoord, Mat(pixCoord).type());
    currentGray = grayMapCam.at(pixCoord);// Find gray value at corner location
    // Find corresponding gray value in projector map and save point 
  }

  return projCorners;
}

/**
 * 
 */
Vec2f ActiveCal::findGrayCoordinate(int graylvl, int nlevels, Size mapSize)
{
  Vec2f grayCoord;
  int i;
  for(i = 1 ; i <= nlevels ; i++)
  {
    graylvl = graylvl / 2;
  }
  return grayCoord;
}

/**
 * 
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