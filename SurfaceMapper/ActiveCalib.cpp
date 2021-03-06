#include "ActiveCalib.hpp"
#include "ActiveStereoMap.hpp"
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
bool ActiveCal::calibrate(VideoCapture c, Size cboardSize, float sqWidth, float sqHeight, int lvls, int ncals)
{
  namedWindow("Preview",1);
  namedWindow("Projector Corners",1);
  Mat cam, camROI, graycam;
  Mat grayMapCamH, grayMapCamV, grayMapProjH, grayMapProjV;
  c >> cam;
  cvtColor(cam, graycam, CV_RGB2GRAY);
  vector<Vec2f> tempCamCorners, tempProjCorners;
  //vector< vector<Vec2f> > camCorners, projCorners;
  //vector< vector<Vec3f> > objCornersGroup;
  //RC = new stereo_rect;
  camCorners.clear();
  projCorners.clear();
  objCornersGroup.clear();
  
  ActiveStereoMap *map = new ActiveStereoMap(c,Size(PROJECTOR_W,PROJECTOR_H));
  
  generateObjectCorners(cboardSize, sqWidth, sqHeight);
  
  if(ncals > 0)
  {
    int i;
    for(i = 0; i<ncals; i++)
    {
      map->showWhite(100);
      bool foundCheckersFlag = false;
      while(true)
      {
	c >> cam; // Capture image
	if(cam.size().width >= PROJECTOR_W && cam.size().height >= PROJECTOR_H)
	{
	  camROI = cam(Rect(ROI_X_OFFSET,ROI_Y_OFFSET,ROI_WIDTH,ROI_HEIGHT)); // Get 640x480 ROI for stereo
	}
	medianBlur(camROI, camROI, 5);
	// Resize the image to match the projector size
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
      map->runMapping(lvls,true);
      printf("Mapping complete\n");
      grayMapCamH = map->getGrayH();	// Get camera horizontal pattern
      grayMapCamV = map->getGrayV();	// Get camera vertical pattern
      grayMapProjH = map->getGrayProjH(); // Get projector horizontal pattern
      grayMapProjV = map->getGrayProjV(); // Get projector vertical pattern
    
      // Reproject corners into projector frame using gray code values
      projCorners.push_back(backprojectPoints(grayMapCamH, grayMapCamV, grayMapProjH, grayMapProjV, 1024/ActiveStereoMap::intPow(2,lvls), tempCamCorners, tempProjCorners));
      
      // Store reprojected corners out to projector buffer
      objCornersGroup.push_back(objCorners);
      
      vector<Vec2f> pcorners = projCorners.back();
      vector<Vec2f> ccorners = camCorners.back();
      Mat qq, rr;
      map->getGrayV().convertTo(rr,CV_8UC1);
      map->getGrayProjV().convertTo(qq,CV_8UC1);
      drawChessboardCorners(rr,cboardSize,Mat(ccorners),true);
      drawChessboardCorners(qq,cboardSize,Mat(pcorners),true);
      imshow("Projector Corners",qq);
      imshow("Preview",rr);
      
      saveCalPointsToFile(objCornersGroup, camCorners, projCorners);
    } 
  }
  else
  {
    // Load cals if available
    if(!readCalPointsFromFile(&objCornersGroup, &camCorners, &projCorners))
      return false;
    else
    {
      printf("ObjSize:%d\nCCSize:%d\nPCSize:%d\n",(uint)objCornersGroup.size(),(uint)camCorners.size(),(uint)projCorners.size());
    }
  }
  
  Size targetSize = Size(640,480);
  Mat camMat, distCam, projMat, distProj;
  vector<Mat> camRvecs, camTvecs, projRvecs, projTvecs;
  printf("ObjCornersGroup = %u\n",(uint)objCornersGroup.at(0).size());
  printf("CamCornersGroup = %u\n",(uint)camCorners.at(0).size());
  printf("ProjCornersGruppe = %u\n",(uint)projCorners.at(0).size());
  printf("Calibrating camera...\n");
  double camReprojErr = calibrateCamera(objCornersGroup,camCorners,targetSize,camMat, distCam, camRvecs, camTvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);// Perform intrinsic calibrations
  printf("Camera Reprojection Error: %f\n",camReprojErr);
  printf("Calibrating projector...\n");
  double projReprojErr = calibrateCamera(objCornersGroup, projCorners,targetSize,projMat, distProj, projRvecs, projTvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
  printf("Projector Reprojection Error: %f\n",projReprojErr);

  RC.MCam = camMat.clone();
  RC.MProj = projMat.clone();
  RC.DistCam = distCam.clone();
  RC.DistProj = distProj.clone();
  
  printf("Intrinsic calibrations complete\n");
  
  Mat camSource;
  Mat projSource;
  if(ncals > 0)
  {
    camSource = camROI;
    projSource = map->getGrayProjV();
  }
  else
  {
    camSource = imread("norris_2.jpg", CV_LOAD_IMAGE_COLOR);
    projSource = camSource.clone();
  }

  // Show calibration
  namedWindow("Camera Calibration",1);
  namedWindow("Projector Calibration",1);
  Mat calCam, calProj;
  undistort(camSource, calCam, RC.MCam, RC.DistCam);
  undistort(projSource, calProj, RC.MProj, RC.DistProj);//cals->MProj,cals->DistProj);
  imshow("Camera Calibration",calCam);
  imshow("Projector Calibration",calProj);
  
  printf("Computing stereo calibration\n");
  // Perform stereo calibration on correspondence points
  /*Mat Rstereo, Tstereo, Estereo, Fstereo;
  double rmsRPE = stereoCalibrate(objCornersGroup, projCorners, camCorners, 
		  cals->MProj, cals->DistProj, cals->MCam, cals->DistCam,
		  targetSize,
		  Rstereo, Tstereo, Estereo, Fstereo,
		  TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,30,1e-6),
		  CALIB_FIX_INTRINSIC);
  printf("Stereo Reprojection Error RMS: %f\n",rmsRPE);
  cals->R = Rstereo.clone();
  cals->T = Tstereo.clone();
  cals->F = Fstereo.clone();*/
  
  // Test stereo rectification
  Mat rectCam, rectProj;
  Mat rectRProj, rectRCam, rectPProj, rectPCam, rectQ;
  Mat camMap[2], projMap[2];
  Mat newCamMat, newProjMat;
  vector<Vec2f> allProjCorners, allCamCorners;
  int iter,in;
  for(iter=0;iter<projCorners.size();iter++)
  {
    for(in=0;in<projCorners.at(iter).size();in++)
    {
      allProjCorners.push_back(projCorners.at(iter).at(in));
      allCamCorners.push_back(camCorners.at(iter).at(in));
    }
  }
  printf("Computing stereo rectification\n");
  
    RC.F = findFundamentalMat(allProjCorners, allCamCorners, FM_8POINT, 0, 0);
    printf("Fundamental matrix found\n");
    Mat H1, H2, P1, P2, R1, R2;
  
    Size rectSize(960,720);
    stereoRectifyUncalibrated(allProjCorners, allCamCorners, RC.F, rectSize, H1, H2, 3);
    printf("Uncal stereo rectification found\n");
    R1 = RC.MProj.inv()*H1*RC.MProj;
    R2 = RC.MCam.inv()*H2*RC.MCam;
    P1 = RC.MProj;
    P2 = RC.MCam;
    printf("Computing undistortion transform\n");
    //Precompute maps for cv::remap()
    initUndistortRectifyMap(RC.MProj, RC.DistProj, R1, P1, rectSize, CV_16SC2, RC.projMap[0], RC.projMap[1]);
    initUndistortRectifyMap(RC.MCam, RC.DistCam, R2, P2, rectSize, CV_16SC2, RC.camMap[0], RC.camMap[1]);

  /*
  stereoRectify(cals->MProj, cals->DistProj, cals->MCam, cals->DistCam,
		targetSize,
		Rstereo, Tstereo, rectRProj, rectRCam, 
		rectPProj, rectPCam, rectQ,
		CALIB_ZERO_DISPARITY,
		-1, targetSize, 0, 0);
  // Generate undistortion maps
  printf("Generating undistorting maps\n");
  initUndistortRectifyMap(cals->MProj, cals->DistProj, rectRProj,
			  newProjMat, targetSize,
			  CV_32FC1,
			  projMap[0], projMap[1]);
  
  initUndistortRectifyMap(cals->MCam, cals->DistCam, rectRCam, 
			  newCamMat , targetSize,
			  CV_32FC1,
			  camMap[0], camMap[1]);
  
  printf("Rectifying images\n");

  */
  printf("Getting images to remap\n");
  Mat rectifiedCam, rectifiedProj;
  rectifiedCam.create(1,1,CV_8UC1);
  rectifiedProj.create(1,1,CV_8UC1);
  Mat gv, gpv;
  gv = imread("norris_2.jpg", CV_LOAD_IMAGE_COLOR);
  gpv = gv.clone();
  //map->getGrayV().convertTo(gv, CV_8UC1);
  //map->getGrayProjV().convertTo(gpv, CV_8UC1);
  printf("Remapping Images\n");
  rectifyImages(gv,gpv,&rectifiedCam,&rectifiedProj);
  //remap(gv,rectifiedCam,RC.camMap[0],RC.camMap[1],CV_INTER_NN,BORDER_CONSTANT,0);
  //remap(gpv,rectifiedProj,RC.projMap[0],RC.projMap[1],CV_INTER_NN,BORDER_CONSTANT,0);
  
  printf("IMSIZE %dx%d\n",rectifiedCam.size().width, rectifiedCam.size().height);

  printf("Displaying rectified images\n");
  namedWindow("Rectified Camera",1);
  namedWindow("Rectified Projector",1);
  imshow("Rectified Camera", rectifiedCam);
  imshow("Rectified Projector", rectifiedProj);
  
  waitKey(0);
  
  return true;
}

/**
 * @brief Calibrates from stored points without displaying images
 */
bool ActiveCal::calibratePassive(void)
{
  if(!readCalPointsFromFile(&objCornersGroup, &camCorners, &projCorners))
      return false;

  Size targetSize = Size(640,480);
  Mat camMat, distCam, projMat, distProj;
  vector<Mat> camRvecs, camTvecs, projRvecs, projTvecs;
  printf("ObjCornersGroup = %u\n",(uint)objCornersGroup.at(0).size());
  printf("CamCornersGroup = %u\n",(uint)camCorners.at(0).size());
  printf("ProjCornersGruppe = %u\n",(uint)projCorners.at(0).size());
  printf("Calibrating camera...\n");
  double camReprojErr = calibrateCamera(objCornersGroup,camCorners,targetSize,camMat, distCam, camRvecs, camTvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);// Perform intrinsic calibrations
  printf("Camera Reprojection Error: %f\n",camReprojErr);
  printf("Calibrating projector...\n");
  double projReprojErr = calibrateCamera(objCornersGroup, projCorners,targetSize,projMat, distProj, projRvecs, projTvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
  printf("Projector Reprojection Error: %f\n",projReprojErr);

  RC.MCam = camMat.clone();
  RC.MProj = projMat.clone();
  RC.DistCam = distCam.clone();
  RC.DistProj = distProj.clone();
  
  printf("Intrinsic calibrations complete\n");
  
  Mat rectCam, rectProj;
  Mat rectRProj, rectRCam, rectPProj, rectPCam, rectQ;
  Mat camMap[2], projMap[2];
  Mat newCamMat, newProjMat;
  vector<Vec2f> allProjCorners, allCamCorners;
  int iter,in;
  for(iter=0;iter<projCorners.size();iter++)
  {
    for(in=0;in<projCorners.at(iter).size();in++)
    {
      allProjCorners.push_back(projCorners.at(iter).at(in));
      allCamCorners.push_back(camCorners.at(iter).at(in));
    }
  }
  printf("Computing stereo rectification\n");
  
    RC.F = findFundamentalMat(allProjCorners, allCamCorners, FM_8POINT, 0, 0);
    printf("Fundamental matrix found\n");
    Mat H1, H2, P1, P2, R1, R2;
  
    stereoRectifyUncalibrated(allProjCorners, allCamCorners, RC.F, targetSize, H1, H2, 2);
    printf("Uncal stereo rectification found\n");
    R1 = RC.MProj.inv()*H1*RC.MProj;
    R2 = RC.MCam.inv()*H2*RC.MCam;
    P1 = RC.MProj;
    P2 = RC.MCam;
    printf("Computing undistortion transform\n");
    //Precompute maps for cv::remap()
    initUndistortRectifyMap(RC.MProj, RC.DistProj, R1, P1, targetSize, CV_16SC2, RC.projMap[0], RC.projMap[1]);
    initUndistortRectifyMap(RC.MCam, RC.DistCam, R2, P2, targetSize, CV_16SC2, RC.camMap[0], RC.camMap[1]);
  
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
  vector<Vec2f> tempCorners;
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
    imgX = (int) (pixCoord[0] + 0.5);
    imgY = (int) (pixCoord[1] + 0.5);
    currentGrayH = grayMapCamH.at<short>(imgY,imgX);// Find gray value at corner location
    currentGrayV = grayMapCamV.at<short>(imgY,imgX);
    projX = cellSize * ActiveStereoMap::grayToBinary(currentGrayV[0]) + cellSize / 2; // Convert to index in projector frame
    projY = cellSize * ActiveStereoMap::grayToBinary(currentGrayH[0]) + cellSize / 2;
    printf("Gray Level is %f,%f at %d,%d\n",currentGrayH[0],currentGrayV[0],imgX,imgY);
    pixCoord = Vec2f(projX,projY);
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


vector<Mat> ActiveCal::getRectificationTransforms(void)
{
  vector<Mat> ov;
  ov.push_back(RC.camMap[0]);
  ov.push_back(RC.camMap[1]);
  ov.push_back(RC.projMap[0]);
  ov.push_back(RC.projMap[1]);
  return ov;
}
/**
 * 
 */
void ActiveCal::rectifyImages(Mat camImg, Mat projImg, Mat *rectCam, Mat *rectProj)
{
  Mat rc, rp;
  remap(camImg,rc,RC.camMap[0],RC.camMap[1],CV_INTER_NN,BORDER_CONSTANT,0);
  remap(projImg,rp,RC.projMap[0],RC.projMap[1],CV_INTER_NN,BORDER_CONSTANT,0);
  *rectCam = rc.clone();
  *rectProj = rp.clone();
  printf("%d\n",rectCam->size().width);
}


/**
 * 
 */
cal_set * ActiveCal::getCals(void)
{
  return cals;
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

/**
 * @brief Converts a floating point number to a string
 * @param f Floating point number to convert to a string
 * @return String representation of f
 */
string ActiveCal::floatToString(float f)
{
  ostringstream buf;
  buf << f;
  return buf.str();
}

/**
 * @brief Converts a string to a float
 * @param s String containing only a string representation of floating point number to convert
 * @return Representation as a float
 */
float ActiveCal::stringToFloat(string s)
{
    float out;
    stringstream buf;
    buf << s;
    buf >> out;
    return out;
}

/**
 * @brief Saves object, camera, and projector checkerboard corner points to a file
 * @param objSet Input array of arrays of corner points in object space (3D-Flat)
 * @param camSet Input array of arrays of corner points in camera space (2D)
 * @param projSet Input array of arrays of corner points in projector space (2D)
 * @return Success of save operation
 */
bool ActiveCal::saveCalPointsToFile(vector<vector<Vec3f> > objSet, vector<vector<Vec2f> > camSet, vector<vector<Vec2f> > projSet)
{
  ofstream calfs;
  printf("Starting calibration point set file write.\n");
  calfs.open("cal_points.set");
  if(!calfs.is_open())
  {
    printf("Error: Calibration point file 'cal_points.set' could not be opened\n");
    fprintf(stderr,"Error: Calibration point file 'cal_points.set' could not be opened\n");
    return false;
  }
  Vec3f c3pt;
  Vec2f c2pt;
  string line1, line2;
  int i,j;
  calfs << "OBJECT\n";
  for(i=0;i<objSet.size();i++)
  {
    for(j=0;j<objSet.at(i).size();j++)
    {
      c3pt = objSet.at(i).at(j);
      calfs << floatToString(c3pt[0]) << "," << floatToString(c3pt[1]) << "," << floatToString(c3pt[2]) << ";";
    }
    calfs << "\n";
    // Print out lists
  }
  calfs << "END_OBJECT\n\n";
  
  calfs << "CAMERA\n";
  for(i=0;i<camSet.size();i++)
  {
    for(j=0;j<camSet.at(i).size();j++)
    {
      c2pt = camSet.at(i).at(j);
      calfs << floatToString(c2pt[0]) << "," << floatToString(c2pt[1]) << ";";
    }
    calfs << "\n";
  }
  calfs << "END_CAMERA\n\n";
  
  calfs << "PROJECTOR\n";
  for(i=0;i<projSet.size();i++)
  {
    for(j=0;j<projSet.at(i).size();j++)
    {
      c2pt = projSet.at(i).at(j);
      calfs << floatToString(c2pt[0]) << "," << floatToString(c2pt[1]) << ";";
    }
    calfs << "\n";
  }
  calfs << "END_PROJECTOR\n\n";
  calfs.close();
  printf("Calibration dataset file write complete\n");
  return true;
}

/**
 * @brief Loads object, camera, and projector checkerboard corner points from a file
 * @param objSet Output array of arrays of corner points in object space (3D-Flat)
 * @param camSet Output array of arrays of corner points in camera space (2D)
 * @param projSet Output array of arrays of corner points in projector space (2D)
 * @return Success of load operation
 */
bool ActiveCal::readCalPointsFromFile(vector<vector<Vec3f> > * objSet, vector<vector<Vec2f> > * camSet, vector<vector<Vec2f> > *projSet)
{
  ifstream calfs;
  printf("Opening and reading calibration points from file\n");
  calfs.open("cal_points.set");
  if(!calfs.is_open())
  {
    printf("Cannot open calibration point file 'cal_points.set'\n");
    fprintf(stderr,"Cannot open calibration point file 'cal_points.set'\n");
    return false;
  }
  string line = "";
  string temp;
  istringstream *sse;
  istringstream *ssi;
  vector<Vec3f> c3vec;
  vector<Vec2f> c2vec;
  Vec3f c3pt;
  Vec2f c2pt;
  int state = -1;
  uint count;
  while(calfs.good())
  {
    getline(calfs,line); // Get line
    
    switch(state)
    {
      case -1:
      {
	if(line.find("OBJECT") != string::npos)
	{
	  state++;
	  count = 0;
	}
	break;
      }
      case 0:
      {
	if(line.find("END_OBJECT") != string::npos)
	{
	  state++;
	  while(line.find("CAMERA") == string::npos)
	  {
	    getline(calfs,line);
	  }
	  count = 0;
	  break;
	}
	else
	{
	 sse = new istringstream(line);
	 while(sse->good())
	 {
	  getline(*sse,temp,';');
	  //cout << temp << endl; // Test print
	  ssi = new istringstream(temp);
	  getline(*ssi,temp,',');
	  c3pt[0] = stringToFloat(temp);
	  getline(*ssi,temp,',');
	  c3pt[1] = stringToFloat(temp);
	  getline(*ssi,temp);
	  c3pt[2] = stringToFloat(temp);
	  //printf("%f,%f,%f\n",c3pt[0],c3pt[1],c3pt[2]);
	  c3vec.push_back(c3pt);
	  free(ssi);
	 }
	 c3vec.pop_back();
	 objSet->push_back(c3vec);
	 c3vec.clear();
	 count++;
	 // Parse into segments by semicolon
	 // Parse into vector elements by comma
	 free(sse);
	}
	break;
      }
      case 1:
      {
	if(line.find("END_CAMERA") != string::npos)
	{
	 state++;
	 while(line.find("PROJECTOR") == string::npos)
	 {
	  getline(calfs,line);
	 }
	 count = 0;
	 break;
	}
	else
	{
	 sse = new istringstream(line);
	 while(sse->good())
	 {
	   getline(*sse,temp,';');
	   //cout << temp << endl;
	   ssi = new istringstream(temp);
	   getline(*ssi,temp,',');
	   c2pt[0] = stringToFloat(temp);
	   getline(*ssi,temp,',');
	   c2pt[1] = stringToFloat(temp);
	   c2vec.push_back(c2pt);
	   free(ssi);
	 }
	 c2vec.pop_back(); // Clear out end of line silliness
	 camSet->push_back(c2vec);
	 c2vec.clear();
	 count++;
	 free(sse);
	}
	break;
      }
      case 2:
      {
	if(line.find("END_PROJECTOR") != string::npos)
	{
	 state++;
	 count = 0;
	 break; 
	}
	else
	{
	 sse = new istringstream(line);
	 while(sse->good())
	 {
	   getline(*sse,temp,';');
	    //cout << temp << endl;
	    ssi = new istringstream(temp);
	    getline(*ssi,temp,',');
	    c2pt[0] = stringToFloat(temp);
	    getline(*ssi,temp,',');
	    c2pt[1] = stringToFloat(temp);
	    c2vec.push_back(c2pt);
	    Vec2f back = c2vec.back();
	    //printf("[%f,%f]\n",c2pt[0],c2pt[1]);
	    //printf("[%f,%f]\n",back[0],back[1]);
	    //printf("Sizeof %d\n",(uint)c2vec.size());
	    free(ssi);
	 }
	 c2vec.pop_back();
	 projSet->push_back(c2vec);
	 c2vec.clear();
	 count++;
	 free(sse);
	}
	break;
      }
      default:
	break;
    }
    
  }
  calfs.close();
  
  Vec3f vvv = objSet->back().back();
  printf("%d V = [%f,%f,%f]\n",(uint)(objSet->back()).size(),vvv[0],vvv[1],vvv[2]);
  Vec2f vv = camSet->back().back();
  printf("%d VV = [%f,%f]\n",(uint)(camSet->back()).size(),vv[0],vv[1]);
  vv = projSet->back().back();
  printf("%d VVV = [%f,%f]\n",(uint)(projSet->back()).size(),vv[0],vv[1]);
  
  printf("File read complete\n");
  return true;
}
