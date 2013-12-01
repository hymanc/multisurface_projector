#include "ProcGen.hpp"


int main(int argc, char ** argv)
{
  int level = atoi(argv[1]);
  namedWindow("Gray", 1);
  Mat testGray = ProcGen::getPattern(Size(640,480),level,HORIZONTAL,false);
  imshow("Gray", testGray);
  waitKey(0);
  return 0;
}