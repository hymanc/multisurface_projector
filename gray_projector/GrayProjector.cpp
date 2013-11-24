#include "GrayProjector.hpp"

string GrayProjector::to_string(int a)
{
    string str;
    ostringstream temp;
    temp<<a;
    return temp.str();
}

GrayProjector::GrayProjector()
{
  // Initialize the patterns
  int i;
  string base_path = "/home/cody/dev/multisurface_projector/gray_projector/patterns/";
  string path_h;
  string path_v;
  Mat tempH, tempV;
  for(i=1;i<=10;i++)
  {
    printf("Starting Loop\n");
    path_h = base_path + to_string(i) +"H.png";
    path_v = base_path + to_string(i) +"V.png";
    tempH = imread(path_h, 1);
    resize(tempH,tempH,Size(640,480),0,0,CV_INTER_NEAREST);
    tempV = imread(path_v, 1);
    resize(tempV,tempV,Size(640,480),0,0,CV_INTER_NEAREST);
    this->grayPatternsH.push_back(tempH);
    this->grayPatternsV.push_back(tempV);
  }
}

Mat GrayProjector::getGrayPattern(int scale, bool h)
{
  if(h)
  {
    return this->grayPatternsH.at(scale-1);
  }
  return this->grayPatternsV.at(scale-1);
}
