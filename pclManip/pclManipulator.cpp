/*
 * pclManipulator.cpp
 * Point Cloud manipulation utility
 * Authors: Jakob Hoellerbauer, Cody Hyman, Scott Meldrum
 * December, 2013
 */

#include "pclManipulator.hpp"

pclManipulator::pclManipulator()
{
  PointCloud<PointNormal>::Ptr temp (new PointCloud<PointNormal>);
  pptr = temp;
  PolygonMesh::Ptr t (new PolygonMesh);
  pmesh = t;
  applyMedian = false;
  applyRadOutRem = false;
  applyPassThrough = false;
  applyStatOutRem = false;
  medianWindowSize = 5;
  filtered = false;
  searchRadius = 20;
  minNeighbors = 20;
  statOutMean = 5;
  statOutStdDev = 1.0;
  minZLimit = -1000;
  maxZLimit = 1000;
  cloudReady = false;
  meshGenerated = false;
}

pclManipulator::~pclManipulator()
{
  delete &pptr;
}

void pclManipulator::initialize(const string & fileName)
{
    struct stat buf;
    if(stat(fileName.c_str(),&buf)==-1)
    {
      cerr << "Warning: ply file given to pclManipulator does not exist" << endl;
      cerr << "No data will be loaded" << endl << endl;
      cloudReady = false;
    }
    else
    {
      io::loadPLYFile(fileName,*pptr);
      if(!pptr->empty())
        cloudReady = true;
      else
      {
        cerr << "ERROR: point cloud not correctly read from file. " << endl << "Point Cloud is currently empty." << endl << endl;
        cloudReady = false;    
      }
    }
}

void pclManipulator::initialize(cv::Mat disparityMap)
{
    vector<float> v;
    vector<float> row;
    cv::Size s = disparityMap.size();

    for(int i=0;i<s.height;i++)
    {
      float* pointer = disparityMap.ptr<float>(i);
      row = vector<float>(pointer,pointer+disparityMap.cols);
      v.insert(v.end(),row.begin(),row.end());
    }
    io::OrganizedConversion<PointNormal,false> OC;   
    std::vector<uint8_t> temp;
    OC.convert(v,temp,false,s.width,s.height,1,*pptr);

    if(!pptr->empty())
      cloudReady = true;
    else
      cloudReady = false;
}
    
void pclManipulator::filterPC()
{
  int filtersApplied = 0;
  if(cloudReady)
  {
    //Apply Median Filter
    if(applyMedian&&pptr->isOrganized())
    {
      PointCloud<PointNormal>::Ptr tempPtr = pptr->makeShared();
      int oldSize = tempPtr->width*tempPtr->height;
      MedianFilter<PointNormal> m;
      m.setInputCloud(tempPtr);
      m.setWindowSize(medianWindowSize);
      PointCloud<PointNormal> ptemp;
      m.filter(ptemp);
      pptr=ptemp.makeShared();
      printUpdate("Median",oldSize);
      filtered = true;
      filtersApplied++;  
    }
    if(applyRadOutRem)
    {
      PointCloud<PointNormal>::Ptr tempPtr = pptr->makeShared();
      int oldSize = tempPtr->width*tempPtr->height;
      cout << "Warning: applying radius outlier removal filter on point cloud" << endl;
      cout << "This will cause the point cloud to become unordered!" << endl << endl;
      RadiusOutlierRemoval<PointNormal> r(true);
      r.setInputCloud(tempPtr);
      r.setRadiusSearch(searchRadius);
      r.setMinNeighborsInRadius(minNeighbors);
      PointCloud<PointNormal> ptemp;
      r.filter(*pptr);
      printUpdate("Radius Outlier Removal",oldSize);
      filtered = true;
      filtersApplied++;
    }
    if(applyStatOutRem)
    {
      PointCloud<PointNormal>::Ptr tempPtr = pptr->makeShared();
      int oldSize = tempPtr->width*tempPtr->height;
      StatisticalOutlierRemoval<PointNormal> sor(true);
      sor.setInputCloud(tempPtr);
      sor.setMeanK(statOutMean);
      sor.setStddevMulThresh(statOutStdDev);
      PointCloud<PointNormal> ptemp;
      sor.filter(ptemp);
      pptr = ptemp.makeShared();  
      printUpdate("Statistical Outlier Removal",oldSize);
      filtered = true;
      filtersApplied++;
    }
    if(applyPassThrough)
    {
      PointCloud<PointNormal>::Ptr tempPtr = pptr->makeShared();
      int oldSize = tempPtr->width*tempPtr->height;
      cout << "Warning: applying passThrough filter on point cloud" << endl;
      cout << "This will cause the point cloud to become unordered!" << endl << endl;  
      PassThrough<PointNormal> pt(true);
      pt.setInputCloud(pptr);
      pt.setFilterFieldName("z");
      pt.setFilterLimits(minZLimit,maxZLimit);
      PointCloud<PointNormal> ptemp;
      pt.filter(ptemp);
      pptr = ptemp.makeShared(); 
      printUpdate("Pass Through",oldSize);
      //pt.~PassThrough<PointNormal>();
      filtered = true;
      filtersApplied++;
    }
    if(filtered==true)
    {
      PointCloud<pcl::PointXYZ>::Ptr t (new pcl::PointCloud<pcl::PointXYZ>);
      copyPointCloud(*pptr,*t);
      NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
      PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
      search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
      tree->setInputCloud (t);
      n.setInputCloud (t);
      n.setSearchMethod (tree);
      n.setKSearch (20);
      n.compute (*normals);
      concatenateFields (*t, *normals, *pptr);
      cout << "Normals have been recomputed for point cloud." << endl << endl;
    }
  }
  cout << filtersApplied << " filters have been applied to the point cloud. " << endl << endl;
}

 void pclManipulator::saveToFile(int fileType, const string & fileName)
{
  cout << "Attempting to save point cloud to file: " << fileName << endl;
  //cout << *pptr << endl;
  int saveSuccessful;
  if(cloudReady)
  {
    if(fileType==1)
    {
      PLYWriter wp;
      PCLPointCloud2 p2temp;
      toPCLPointCloud2(*pptr, p2temp);
      saveSuccessful = wp.write(fileName, p2temp,Eigen::Vector4f::Zero(),  Eigen::Quaternionf::Identity(), false, false);
      //io::savePLYFileASCII(fileName,*pptr);
    }
    else if(fileType==2)
      saveSuccessful = io::savePCDFileASCII(fileName,*pptr);
    else
      cerr << "ERROR: fileType not recognized "     \
           << endl << "Use 1 to save as PLY file "  \
           << endl << "Use 2 to save as PCD file " << endl;
  }
  else
      cerr << "ERROR: point cloud contains no data!" << endl << "No save file will be created" << endl;

  if(saveSuccessful==0)
    cout << "Save successful!" << endl << endl;
  else
    cout << "ERROR: Save unsuccessful!" << endl << endl;
  //cout << *pptr << endl;
}

void pclManipulator::generateMesh()
{
  if(cloudReady)
  {
    search::KdTree<pcl::PointNormal>::Ptr tree2 (new pcl::search::KdTree<pcl::PointNormal>);
    tree2->setInputCloud (pptr);
    pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
    // Set the maximum distance between connected points (maximum edge length)
    gp3.setSearchRadius (9);
    // Set typical values for the parameters
    gp3.setMu(60.0);
    gp3.setMaximumNearestNeighbors (180);
    gp3.setMaximumSurfaceAngle(M_PI/4); // 45 degrees
    gp3.setMinimumAngle(M_PI/18); // 10 degrees
    gp3.setMaximumAngle(2*M_PI/3); // 120 degrees
    gp3.setNormalConsistency(false);

    gp3.setInputCloud (pptr);
    gp3.setSearchMethod (tree2);
    gp3.reconstruct (*pmesh);
    meshGenerated = true;
    
  }
}

void pclManipulator::saveMesh(const string & fileName)
{
  if(!meshGenerated)
  {
    cout << "ERROR: Mesh has not been generated from point cloud. " << endl;
    cout << "ERROR: No .obj file will be created. " << endl << endl;
  }
  else
  {
    if(io::saveOBJFile(fileName,*pmesh,6)==0)
    {
      io::savePLYFile("best_mesh.ply",*pmesh,6);
      cout << "mesh saved to .obj file successfully!" << endl << endl;
    }
    else
      cout << "ERROR: Mesh save unsuccessful! " << endl << endl;
  }
}    

void pclManipulator::smoothMesh(int numIter, float relax)
{
  if(!meshGenerated)
  {
    cout << "ERROR: Mesh has not been generatore from point cloud. " << endl;
    cout << "ERROR: Smoothing operation requested will not take place. " << endl << endl;
    return;
  }

  MeshSmoothingLaplacianVTK v;
  v.setInputMesh(pmesh);
  v.setNumIter(numIter);
  v.setRelaxationFactor(relax);
  PolygonMesh::Ptr t (new PolygonMesh);
  v.process(*t);
  pmesh = t;
}
  
  

void pclManipulator::setApplyMedian(bool in)
{
  applyMedian = in;
}

void pclManipulator::setApplyRadOutRem(bool in)
{
  applyRadOutRem = in;
}

void pclManipulator::setApplyPassThrough(bool in)
{
  applyPassThrough = in;
}

void pclManipulator::setApplyStatOutRem(bool in)
{
  applyStatOutRem = in;
}

void pclManipulator::setMedianWindowSize(int winSize)
{
  medianWindowSize = winSize;
}

void pclManipulator::setSearchRadius(double rad)
{
  searchRadius = rad;
}

void pclManipulator::setMinNeighbors(int mN)
{
  minNeighbors = mN;
}

void pclManipulator::setZLimits(double min, double max)
{
  minZLimit = min;
  maxZLimit = max;
}

void pclManipulator::setStatParams(double mean, double stdDev)
{
  statOutMean = mean;
  statOutStdDev = stdDev;
}

bool pclManipulator::getApplyMedian()
{
  return applyMedian;
}

bool pclManipulator::getApplyRadOutRem()
{
  return applyRadOutRem;
}

bool pclManipulator::getApplyPassThrough()
{
  return applyPassThrough;
}

bool pclManipulator::getApplyStatOutRem()
{
  return applyStatOutRem;
}

int pclManipulator::getMedianWindowSize(int winSize)
{
  return medianWindowSize;
}

double pclManipulator::getSearchRadius()
{
  return searchRadius;
}

int pclManipulator::getMinNeighbors()
{
  return minNeighbors;
}

void pclManipulator::printUpdate(const string & filter, int oldSize)
{
  int newSize = pptr->width*pptr->height;
  cout << filter << " filter has been applied to point cloud." << endl;
  cout << oldSize-newSize << " Points have been removed." << endl;
  cout << "Point Cloud now has " << newSize << " points." << endl << endl;
}


/*
array<double,2> pclManipulator::getZLimits()
{
  double ret[2] = {minZLimit, maxZLimit};
  return ret;
*/




