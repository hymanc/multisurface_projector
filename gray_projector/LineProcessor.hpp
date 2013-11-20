
#include <opencv/opencv.hpp>
#include <iostream>

using namespace cv;

public class LineProcessor
{
public:
  LineProcessor();
  virtual ~LineProcessor();
  Mat extractLines(Mat inImg);
private:
  Mat buffer;
};