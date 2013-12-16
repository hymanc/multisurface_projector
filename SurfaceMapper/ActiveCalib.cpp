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
  vector<Vec2f> tempCamCorners, tempProjCorners;
  vector< vector<Vec2f> > camCorners, projCorners;
  vector< vector<Vec3f> > objCornersGroup;
  ActiveStereoMap *map = new ActiveStereoMap(c,Size(PROJECTOR_W,PROJECTOR_H));
  
  generateObjectCorners(cboardSize, sqWidth, sqHeight);
  
  int i;
  for(i = 0; i<ncals; i++)
  {
    map->showWhite(100);
    bool foundCheckersFlag = false;
    // Wait for 
    //while(waitKey(30) <= 0 || !foundCheckersFlag)
    while(true)
    {
      //c >> cam;
      c >> cam; // Capture image
      if(cam.size().width >= PROJECTOR_W && cam.size().height >= PROJECTOR_H)
      {
	camROI = cam(Rect(ROI_X_OFFSET,ROI_Y_OFFSET,ROI_WIDTH,ROI_HEIGHT)); // Get 640x480 ROI for stereo
      }
      medianBlur(camROI, camROI, 5);
      // Resize here
      resize(camROI,camROI,Size(640,480),0,0,CV_INTER_CUBIC);
      cvtColor(camROI,graycam, CV_RGB2GRAY);
      
      // Find Chesskerboard corners
      foundCheckersFlag = findChessboardCorners(graycam, cboardSize, tempCamCorners,CALIB_CB_ADAPTIVE_THRESH);
      // If found, refine estimates and print corners
      if(foundCheckersFlag)
      {
	printf("Found checkerboard\n");
	cornerSubPix(graycam, tempCamCorners, Size(11,11), Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,30,0.1));
	printPoints(tempCamCorners);
      }
      // Draw over image
      drawChessboardCorners(camROI, cboardSize, Mat(tempCamCorners),foundCheckersFlag); 
      imshow("Preview",camROI);
      if(waitKey(1) >= 0 && foundCheckersFlag) // Wait for keypress
      {
	printf("User input detected:\nRunning Graymap Backprojection\n");
	break;
      }
    }
    camCorners.push_back(tempCamCorners);
    // Run through mapping pattern
    map->runMapping(lvls);
    printf("Mapping complete\n");
    grayMapCamH = map->getGrayH();	// Get camera horizontal pattern
    grayMapCamV = map->getGrayV();	// Get camera vertical pattern
    grayMapProjH = map->getGrayProjH(); // Get projector horizontal pattern
    grayMapProjV = map->getGrayProjV(); // Get projector vertical pattern
    
    //map->printGrayHSize();
    /*namedWindow("GrayTest",1);
    Mat visMat = Mat::zeros(grayMapCamH.size(),CV_8UC1);
    (grayMapCamH).convertTo(visMat, CV_8UC1, 1, 0);
    imshow("GrayTest",255*visMat);
    while(waitKey(0) < 0);*/
  
    // Reproject corners into projector frame using gray code values
    projCorners.push_back(backprojectPoints(grayMapCamH, grayMapCamV, grayMapProjH, grayMapProjV, 1024/ActiveStereoMap::intPow(2,lvls), tempCamCorners, tempProjCorners));
    printf("Proj Returned Group = %u\n",(uint)tempCamCorners.size());

    // Store reprojected corners out to projector buffer
    //projCorners.push_back();//insert(projCorners.end(),tempCorners.begin(),tempCorners.end());
    objCornersGroup.push_back(objCorners);
  } 
  // Rescale points for 640x480
  
  Mat camMat, distCam, projMat, distProj;
  vector<Mat> camRvecs, camTvecs, projRvecs, projTvecs;
  printf("ObjCornersGroup = %u\n",(uint)objCornersGroup.at(0).size());
  printf("CamCornersGroup = %u\n",(uint)camCorners.at(0).size());
  printf("ProjCornersGruppe = %u\n",(uint)projCorners.at(0).size());
  printf("Calibrating camera...\n");
  double camReprojErr = calibrateCamera(objCornersGroup,camCorners,Size(640,480),camMat, distCam, camRvecs, camTvecs,0);// Perform intrinsic calibrations
  printf("Camera Reprojection Error: %f\n",camReprojErr);
  printf("Calibrating projector...\n");
  double projReprojErr = calibrateCamera(objCornersGroup, projCorners,Size(640,480),projMat, distProj, projRvecs, projTvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
  printf("Projector Reprojection Error: %f\n",projReprojErr);
  
  // Show calibration
  namedWindow("Calibration",1);
  Mat calCam;
  undistort(camROI, calCam, camMat, distCam);
  imshow("Calibration",calCam);
  
  // Perform stereo calibration on correspondence points
  /*stereoCalibrate(objectPoints, allCamCorners, allProjCorners,
		  cmat1, dcoeff1, cmat2, dcoeff2, map->getPatternSize(),
		  
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
vector<Vec2f> ActiveCal::backprojectPoints(Mat grayMapCamH, Mat grayMapCamV, Mat grayMapProjH, Mat grayMapProjV, int cellSize, vector<Vec2f> camCorners, vector<Vec2f> projCorners)
{
  //vector<Vec2f> projCorners;
  vector<Vec2f> tempCorners;
  //projCorners.clear();
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
    currentGrayH = grayMapCamH.at<short>(imgY,imgX);// Find gray value at corner location
    currentGrayV = grayMapCamV.at<short>(imgY,imgX);
    projX = cellSize * ActiveStereoMap::grayToBinary(currentGrayV[0]) + cellSize / 2; // Convert to index in projector frame
    projY = cellSize * ActiveStereoMap::grayToBinary(currentGrayH[0]) + cellSize / 2;
    printf("Gray Level is %f,%f at %d,%d\n",currentGrayH[0],currentGrayV[0],imgX,imgY);
    pixCoord = Vec2f(projY,projX);
    tempCorners.push_back(pixCoord);
    //ActiveCal::grayToBinary(currentGray);
    // Find corresponding gray value in projector map and save point 
  }
  projCorners = tempCorners;
  printf("TempCornerSize = %u\n",(uint)tempCorners.size());
  return tempCorners;
}

/**
 * @brief Generates the world-space coordinates of the checkerboard corners
 * @param cboardSize The number of inner corners on the pattern
 * @param sqWidth The metric width of the checkerboard squares
 * @param sqHeight The metric height of the checkerboard squares
 */
void ActiveCal::generateObjectCorners(Size cboardSize, float sqWidth, float sqHeight)
{
  int i,j;
  Vec3f ptCoord;
  for(j=0;j<cboardSize.height;j++) // Y-coordinate outer loop
  {
   for(i=0;i<cboardSize.width;i++) // X-coordinate inner loop
   {
     ptCoord[0] = i*sqWidth;
     ptCoord[1] = j*sqHeight;
     ptCoord[2] = 0; // Planar constraint
     objCorners.push_back(ptCoord);
   }
  }
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