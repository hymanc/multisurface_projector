#include "pclManipulator.hpp"


int main(int argc, char ** argv)
{
  pclManipulator *pclMan = new pclManipulator();
  pclMan->initialize("boxwall.ply");
  pclMan->setApplyRadOutRem(true);
  pclMan->setApplyStatOutRem(true);
  pclMan->setApplyPassThrough(true);
  pclMan->setSearchRadius(15);
  pclMan->setMinNeighbors(20);
  pclMan->filterPC();
  cout << "Successfully exited filterPC() function! YAY!" << endl;
  pclMan->saveToFile(1,"boxwall_filtered.ply");
  cout << "Successfully exited saveToFile() function! YAY!" << endl;
  pclMan->generateMesh();
  cout << "Successfully exited from generateMesh() function! YAY! " << endl;
  pclMan->smoothMesh(100, 0.2);
  cout << "Successfully exited smoothMesh() function! YAY! " << endl;
  pclMan->saveMesh("best_mesh.obj");
  cout << "Successfully exited from saveMesh() function! YAY! " << endl;
  return 0;
}

