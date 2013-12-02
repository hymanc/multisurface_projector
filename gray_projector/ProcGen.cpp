#include "ProcGen.hpp"

Mat ProcGen::getPattern(Size imSize, uint level, uint direction, bool inverted)
{
  int x = imSize.width;
  int y = imSize.height;
  int nextX = pow(2,ceil(log(x)/log(2)));
  int nextY = pow(2,ceil(log(y)/log(2)));
  int sqSize;
  if(nextX > nextY)
    sqSize = nextX;
  else
    sqSize = nextY;
  printf("Generating square version w/ size %d\n",sqSize);
  Mat workImg = Mat::zeros(Size(sqSize,sqSize),CV_8UC3); // Create zero image
  
  uint lineWidth, iterator, itEnd;
  if(direction == HORIZONTAL)
    lineWidth = sqSize/pow(2,level);
  else 
    lineWidth = sqSize/pow(2,level);
  if(level >= 2) 
      itEnd = pow(2,level-2);
  else
    itEnd = 1;
  
  printf("Using linewidth %d\n",lineWidth);
  for(iterator = 0; iterator < itEnd; iterator++)
  {
    // Draw white bars
    uint start = lineWidth * (1 + iterator*4);
    uint end   = start + 2*lineWidth;
    
    printf("Drawing bar from :%d to %d\n",start,end);
    drawLine(workImg, start, end, direction);
  }
  printf("Number of white bars: %d\n",itEnd);
  
  // Crop
  Mat retImg;
  cvtColor(workImg(Rect(0,0,x-1,y-1)),retImg, CV_RGB2GRAY);
  return retImg;
}


void ProcGen::drawLine(Mat img, uint start, uint end, uint direction)
{
  Scalar color = Scalar(255,255,255);
  int lineType = 8;
  if(direction == VERTICAL)
  {
       rectangle(img,
		 Point(start,0),
		 Point(end-1,img.size().height),
		 color,
		 CV_FILLED,
		 8,
		 0);
  }
  else
  { // Horizontal bar
      rectangle(img,
		Point(0,start),
		Point(img.size().width,end-1),
		color,
		CV_FILLED,
		8,
		0);
  }
}