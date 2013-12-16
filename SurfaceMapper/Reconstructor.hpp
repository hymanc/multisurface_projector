#include "ActiveCalib.hpp"
#include "ActiveStereoMap.hpp"

using namespace std;
using namespace cv;

class Reconstructor
{
public:
  Reconstructor();
  bool collectMap(int nlvls);
  void generateRectificationTransforms(void);
  void rectifyImages(void);
  void computeDisparity(Mat dst);
  void viewRectifiedImages(void);
  void runCalibration(int ntimes);
private:
  
  // 
  ActiveCal *cal;
  ActiveStereoMap *map;
  
  // Calibration
  cal_set *cals;
  
  // Image Storage
  Mat grayCamV;
  Mat grayCamH;
  Mat grayProjV;
  Mat grayProjH;
  
  Mat disparity;
};