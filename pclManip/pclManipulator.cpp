/*
 * pclManipulator.cpp
 * Point Cloud manipulation utility
 * Authors: Jakob Hoellerbauer, Cody Hyman, Scott Meldrum
 * December, 2013
 */

#include "pclManipulator.hpp"

pclManipulator::pclManipulator()
{
  pcl::PointCloud<PointNormal>::Ptr temp (new PointCloud<PointNormal>);
  pptr = temp;
  pcl::PolygonMesh::Ptr t (new PolygonMesh);
  pmesh = t;
  applyMedian = false;
  applyRadOutRem = false;
  applyPassThrough = false;
  applyStatOutRem = false;
  medianWindowSize = 5;
  filtered = false;
  searchRadius = 20;
  minNeighbors = 10;
  statOutMean = 50;
  statOutStdDev = 0.75;
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
    vector<uint16_t> v;
    vector<uint16_t> row;
    cv::Mat temp;
    cv::Size s = disparityMap.size();
    disparityMap.convertTo(temp, CV_16UC1);
    imshow("AWESOME", temp);
    imshow("YEAH!", disparityMap);
    cv::waitKey(0);

    for(int i=0;i<s.height;i++)
    {
      uint16_t* pointer = temp.ptr<uint16_t>(i);
      row = vector<uint16_t>(pointer,pointer+temp.cols);
      v.insert(v.end(),row.begin(),row.end());
    }

    io::OrganizedConversion<PointNormal,false> OC;   
    std::vector<uint8_t> t;
    OC.convert(v,t,false,s.width,s.height,1,*pptr);

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
      pcl::PointCloud<PointNormal>::Ptr tempPtr = pptr->makeShared();
      int oldSize = tempPtr->width*tempPtr->height;
      pcl::MedianFilter<PointNormal> m;
      m.setInputCloud(tempPtr);
      m.setWindowSize(medianWindowSize);
      pcl::PointCloud<PointNormal> ptemp;
      m.filter(ptemp);
      pptr=ptemp.makeShared();
      printUpdate("Median",oldSize);
      filtered = true;
      filtersApplied++;  
    }
    if(applyRadOutRem)
    {
      pcl::PointCloud<PointNormal>::Ptr tempPtr = pptr->makeShared();
      int oldSize = tempPtr->width*tempPtr->height;
      cout << "Warning: applying radius outlier removal filter on point cloud" << endl;
      cout << "This will cause the point cloud to become unordered!" << endl << endl;
      pcl::RadiusOutlierRemoval<PointNormal> r(true);
      r.setInputCloud(tempPtr);
      r.setRadiusSearch(searchRadius);
      r.setMinNeighborsInRadius(minNeighbors);
      pcl::PointCloud<PointNormal> ptemp;
      r.filter(*pptr);
      printUpdate("Radius Outlier Removal",oldSize);
      filtered = true;
      filtersApplied++;
    }
    if(applyStatOutRem)
    {
      pcl::PointCloud<PointNormal>::Ptr tempPtr = pptr->makeShared();
      int oldSize = tempPtr->width*tempPtr->height;
      pcl::StatisticalOutlierRemoval<PointNormal> sor(true);
      sor.setInputCloud(tempPtr);
      sor.setMeanK(statOutMean);
      sor.setStddevMulThresh(statOutStdDev);
      pcl::PointCloud<PointNormal> ptemp;
      sor.filter(ptemp);
      pptr = ptemp.makeShared();  
      printUpdate("Statistical Outlier Removal",oldSize);
      filtered = true;
      filtersApplied++;
    }
    if(applyPassThrough)
    {
      pcl::PointCloud<PointNormal>::Ptr tempPtr = pptr->makeShared();
      int oldSize = tempPtr->width*tempPtr->height;
      cout << "Warning: applying passThrough filter on point cloud" << endl;
      cout << "This will cause the point cloud to become unordered!" << endl << endl;  
      pcl::PassThrough<PointNormal> pt(true);
      pt.setInputCloud(pptr);
      pt.setFilterFieldName("z");
      pt.setFilterLimits(minZLimit,maxZLimit);
      pcl::PointCloud<PointNormal> ptemp;
      pt.filter(ptemp);
      pptr = ptemp.makeShared(); 
      printUpdate("Pass Through",oldSize);
      //pt.~PassThrough<PointNormal>();
      filtered = true;
      filtersApplied++;
    }
    if(filtered==true)
    {
/*    pcl::PointCloud<pcl::PointXYZ>::Ptr t (new pcl::PointCloud<pcl::PointXYZ>);
      pcl::copyPointCloud(*pptr,*t);
      pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
      pcl::PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
      pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
      tree->setInputCloud (t);
      n.setInputCloud (t);
      n.setSearchMethod (tree);
      n.setKSearch (20);
      n.compute (*normals);
      pcl::concatenateFields (*t, *normals, *pptr);
      cout << "Normals have been recomputed for point cloud." << endl << endl;
*/
      pcl::PointCloud<pcl::PointXYZ>::Ptr t2 (new pcl::PointCloud<pcl::PointXYZ>);
      pcl::copyPointCloud(*pptr,*t2);

      // Create a KD-Tree
      pcl::search::KdTree<pcl::PointXYZ>::Ptr tree2 (new pcl::search::KdTree<pcl::PointXYZ>);
      tree2->setInputCloud(t2);
      // Output has the PointNormal type in order to store the normals calculated by MLS
      pcl::PointCloud<pcl::PointNormal> mls_points;

      // Init object (second point type is for the normals, even if unused)
      pcl::MovingLeastSquares<pcl::PointXYZ, pcl::PointNormal> mls;
     
      mls.setComputeNormals (true);

      // Set parameters
      mls.setInputCloud (t2);
      mls.setDilationIterations(2);
      mls.setDilationVoxelSize(3.75);
      mls.setPolynomialOrder(2);
      mls.setPolynomialFit (true);
      mls.setSearchMethod (tree2);
      mls.setUpsamplingMethod((pcl::MovingLeastSquares<pcl::PointXYZ, pcl::PointNormal>::UpsamplingMethod)4); //VOXEL_GRID_DILATION --> was having trouble with enum declaration
      mls.setSearchRadius (20);
      // Reconstruct
      mls.process (*pptr);

      pcl::PointCloud<PointNormal>::Ptr tempPtr = pptr->makeShared();
      int oldSize = tempPtr->width*tempPtr->height;
      cout << "Warning: applying passThrough filter on point cloud" << endl;
      cout << "This will cause the point cloud to become unordered!" << endl << endl;  
      pcl::PassThrough<PointNormal> pt(true);
      pt.setInputCloud(pptr);
      pt.setFilterFieldName("z");
      pt.setFilterLimits(minZLimit,maxZLimit);
      pcl::PointCloud<PointNormal> ptemp;
      pt.filter(ptemp);
      pptr = ptemp.makeShared(); 
      printUpdate("Pass Through",oldSize);

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
      pcl::PLYWriter wp;
      pcl::PCLPointCloud2 p2temp;
      pcl::toPCLPointCloud2(*pptr, p2temp);
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
    pcl::search::KdTree<pcl::PointNormal>::Ptr tree2 (new pcl::search::KdTree<pcl::PointNormal>);
    tree2->setInputCloud (pptr);
    pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
    // Set the maximum distance between connected points (maximum edge length)
    gp3.setSearchRadius (5);
    // Set typical values for the parameters
    gp3.setMu(35);
    gp3.setMaximumNearestNeighbors (200);
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
    if(pcl::io::saveOBJFile(fileName,*pmesh,6)==0)
    {
      pcl::io::savePLYFile("best_mesh.ply",*pmesh,6);
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
  pcl::MeshSmoothingLaplacianVTK v;
  v.setInputMesh(pmesh);
  v.setNumIter(numIter);
  v.setRelaxationFactor(relax);
  pcl::PolygonMesh::Ptr t (new PolygonMesh);
  v.process(*t);
  pmesh = t;

}

void pclManipulator::fillVectors(
      std::vector<glm::vec3> & vertices,
      std::vector<glm::vec2> & uvs,
      std::vector<glm::vec3> & normals)
{
  if(!meshGenerated)
  {
    cout << "ERROR: Mesh has not been generatore from point cloud. " << endl;
    cout << "ERROR: vectors will not be filled . " << endl << endl;
    return;
  }

  glm::vec3 vertTemp;
  glm::vec3 normTemp;
  pcl::PointNormal tempPoint;
  pcl::PointCloud<PointXYZ>  tempCloud;
  pcl::PointCloud<PointNormal> normalXYZ;
  pcl::fromPCLPointCloud2(pmesh->cloud, tempCloud);


  //UGLY UGLY UGLY hack to regenerate the normals. which smoothing apparently destroys
  pcl::PointCloud<pcl::PointXYZ>::Ptr t (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::copyPointCloud(*pptr,*t);
  pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
  pcl::PointCloud<pcl::Normal>::Ptr npc (new pcl::PointCloud<pcl::Normal>);
  pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
  tree->setInputCloud (t);
  n.setInputCloud (t);
  n.setSearchMethod (tree);
  n.setKSearch (20);
  n.compute (*npc);
  pcl::concatenateFields (*t, *npc, normalXYZ);

  for(vector< pcl::Vertices >::iterator it = pmesh->polygons.begin(); it!=pmesh->polygons.end();++it)
  {
    for (size_t i = 0; i < it->vertices.size(); ++i)
    {
      tempPoint =  normalXYZ[it->vertices[i]];
      normTemp.x = tempPoint.normal_x;
      normTemp.y = tempPoint.normal_y;
      normTemp.z = tempPoint.normal_z;
      vertTemp.x = tempPoint.x;
      vertTemp.y = tempPoint.y;
      vertTemp.z = tempPoint.z;
      vertices.push_back(vertTemp);
      normals.push_back(normTemp);
      glm::vec2 uv = glm::vec2(0,0);
      uvs.push_back(uv);
    }
  }
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




