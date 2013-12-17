#ifndef PCL_MANIPULATOR_H
#define PCL_MANIPULATOR_H

#define PCL_NO_PRECOMPILE

#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <sys/stat.h>
#include <vector>

#include <glm/glm.hpp>

#include <pcl/point_types.h>
#include <pcl/compression/organized_pointcloud_conversion.h>
#include <pcl/pcl_base.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/features/normal_3d.h>
#include <pcl/surface/gp3.h>
#include <pcl/Vertices.h>

#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/obj_io.h>

#include <pcl/filters/filter.h>
#include <pcl/filters/median_filter.h>
#include <pcl/filters/radius_outlier_removal.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/statistical_outlier_removal.h>

#include <pcl/surface/vtk_smoothing/vtk_mesh_smoothing_laplacian.h>
#include <pcl/surface/processing.h>
#include <pcl/surface/mls.h>

/*
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
*/


using namespace pcl;
using namespace std;

class pclManipulator
{
public:
  pclManipulator();
  ~pclManipulator();
  void initialize(const string & fileName);
  void initialize(cv::Mat disparityMap);
  void filterPC();
  void saveToFile(int fileType, const string & fileName);
  void generateMesh();
  void saveMesh(const string & fileName);
  void smoothMesh(int numIter, float relax);
  void fillVectors(
      std::vector<glm::vec3> & vertices,
      std::vector<glm::vec2> & uvs,
      std::vector<glm::vec3> & normals);
  

  //Set Functions
  void setApplyMedian(bool in);
  void setApplyRadOutRem(bool in);
  void setApplyStatOutRem(bool in);
  void setApplyPassThrough(bool in);
  void setMedianWindowSize(int winSize);
  void setSearchRadius(double rad);
  void setMinNeighbors(int nM);
  void setZLimits(double min, double max);
  void setStatParams(double mean, double stdDev);

  //Get Functions 
  bool getApplyMedian();
  bool getApplyRadOutRem();
  bool getApplyStatOutRem();
  bool getApplyPassThrough();
  int getMedianWindowSize(int winSize);
  bool isFiltered();
  double getSearchRadius();
  int getMinNeighbors();
  //array<double,2> getZLimits();

private:
  PointCloud<PointNormal>::Ptr pptr;
  PolygonMesh::Ptr pmesh;
  bool applyMedian;
  bool applyRadOutRem;
  bool applyPassThrough;
  bool applyStatOutRem;
  bool filtered;
  bool cloudReady;
  bool meshGenerated;
  double searchRadius;
  int minNeighbors;
  int medianWindowSize;
  double minZLimit;
  double maxZLimit;
  double statOutMean;
  double statOutStdDev;  

  void printUpdate(const string & filter, int oldSize);

};

#endif
