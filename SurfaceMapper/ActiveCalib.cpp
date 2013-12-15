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
  Mat cam, camROI, graycam;
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
      //c >> cam;
      //c >> cam;
      c >> cam;
      c >> cam; // Capture image
      if(cam.size().width >= PROJECTOR_W && cam.size().height >= PROJECTOR_H)
      {
	camROI = cam(Rect(120,160,800,600)); // Get 640x480 ROI for stereo
      }
      cvtColor(camROI,graycam, CV_RGB2GRAY);
      foundCheckersFlag = false;
      //foundCheckersFlag = findChessboardCorners(graycam, cboardSize, camCorners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
      if(foundCheckersFlag)
      {
	printf("Found checkerboard\n");
	cornerSubPix(graycam, camCorners, Size(11,11), Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,30,0.1));
	//printPoints(camCorners);
      }
      // Draw over image
      drawChessboardCorners(camROI, cboardSize, Mat(camCorners),foundCheckersFlag); 
      imshow("Preview",camROI);
      if(waitKey(1) >= 0) // Wait for keypress
	return false;
    }
    // Run through mapping pattern
    map->runMapping(lvls);
    grayMapCamH = map->getGrayH();	// Get camera horizontal pattern
    grayMapCamV = map->getGrayV();	// Get camera vertical pattern
    grayMapProjH = map->getGrayProjH(); // Get projector horizontal pattern
    grayMapProjV = map->getGrayProjV(); // Get projector vertical pattern
    
    // Reproject corners into projector frame using gray code values
    projCorners = backprojectPoints(grayMapCamH, grayMapCamV, grayMapProjH, grayMapProjV, 1024/ActiveStereoMap::intPow(2,lvls), camCorners);
    // Store corners out to larger set if valid
  } 
  // Perform stereo calibration on correspondence points
  /*stereoCalibrate(objectPoints, allCamCorners, allProjCorners,
		  cmat1, dcoeff1, cmat2, dcoeff2, imSize,
		  
		  */
  return true;
}

/**
 * @brief Finds the corresponding set of points in the projector frame
 * @param grayMapCamH Horizontal gray pattern camera image
 * @param grayMapCamV Vertical gray pattern camera image
 * @param grayMapProjH Horizontal gray pattern projector image
 * @param grayMapProjV Vertical gray pattern projector image
 * @param cellSize Size of a graymap row or column in pixels in the projector frame
 * @param camCorners Detected checkerboard corners in camera image
 * @return Checkerboard corner locations in the projector frame
 */
vector<Vec2f> ActiveCal::backprojectPoints(Mat grayMapCamH, Mat grayMapCamV, Mat grayMapProjH, Mat grayMapProjV, int cellSize, vector<Vec2f> camCorners)
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
    projX = cellSize * ActiveStereoMap::grayToBinary(currentGrayV[0]) + cellSize / 2; // Convert to index in projector frame
    projY = cellSize * ActiveStereoMap::grayToBinary(currentGrayH[0]) + cellSize / 2;
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