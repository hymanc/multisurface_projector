#include "CameraCal.h"


int main(int argc, char ** argv)
{
  CameraCal *cal = new CameraCal(new VideoCapture(0));
  cal->calibrate(20);
  return 0;
}