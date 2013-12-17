#include "pclManipulator.hpp"


int main(int argc, char ** argv)
{
  pclManipulator *pclMan = new pclManipulator();
  cv::Mat disparityMap = cv::imread("disparity.bmp",CV_LOAD_IMAGE_GRAYSCALE);
  //pclMan->initialize("boxwall.ply");
  pclMan->initialize(disparityMap);
  pclMan->saveToFile(1,"unfilteredDisparityMapCloud.ply");
  pclMan->setApplyRadOutRem(true);
  pclMan->setApplyStatOutRem(true);
  pclMan->setApplyPassThrough(true);
  pclMan->setSearchRadius(18);
  pclMan->setMinNeighbors(18);
  pclMan->filterPC();
  cout << "Successfully exited filterPC() function! YAY!" << endl;
  pclMan->saveToFile(1,"disparityImageFiltered.ply");
  cout << "Successfully exited saveToFile() function! YAY!" << endl;
  pclMan->generateMesh();
  cout << "Successfully exited from generateMesh() function! YAY! " << endl;
  pclMan->smoothMesh(40, 0.2);
  cout << "Successfully exited smoothMesh() function! YAY! " << endl;
  pclMan->saveMesh("best_mesh.obj");
  cout << "Successfully exited from saveMesh() function! YAY! " << endl;

  vector<glm::vec3> vertices;
  vector<glm::vec3> normals;
  vector<glm::vec2> uvs;
  pclMan->fillVectors(vertices,uvs,normals);
  cout << "Successfully exited from fillVectors() function! YAY! " << endl;
  return 0;
}

