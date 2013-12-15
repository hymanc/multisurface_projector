#ifndef PROC_GEN
#define PROC_GEN

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <opencv2/opencv.hpp>

#define VERTICAL 0
#define HORIZONTAL 1

#define PROJECTOR_W 640
#define PROJECTOR_H 480

typedef unsigned int uint;
using namespace cv;

class ProcGen
{
public:
  static Mat getPattern(Size imSize, uint level, uint direction, bool inverted);
  static uint getMinimumStripeWidth(Size imSize, uint nlvls);
private:
  static void drawLine(Mat img, uint start, uint end, uint direction);
};

#endif

