#include "colorGenerator.h"

using namespace cv;
using namespace std;

void colorGenerator::MyLine( Mat img, int index, bool vertical, Scalar color, int thickness)
{
//    int thickness = 2;
  int lineType = 8;
  if(vertical)
  {
       line( img,
       Point(index,0),
       Point(index,height),
       color,
       thickness,
       lineType );
  }
  else
  {
	line( img,
	Point(0,index),
	Point(width,index),
	color,
        thickness,
        lineType );
  }
}


colorGenerator::colorGenerator(int w, int h, String fname)
{
  width=w;
  height=h;
  filename=fname;
  pattern = Mat::zeros( h, w, CV_8UC3 );
}
colorGenerator::colorGenerator(int w, int h)
{
  colorGenerator(h,w,"Pattern");
}

void colorGenerator::saveToFile()
{
  if(!pattern.empty())
    imwrite( filename, pattern );
  else
  {
    cout << "The object's pattern is empty" << endl;
    cout << "No file of name: " << filename << " will be saved" << endl;
    cout << "Generate the pattern using generatePattern()" << endl;
  }
}
void colorGenerator::setWidth(int w) {width = w;}
void colorGenerator::setHeigth(int h) {height = h;}
void colorGenerator::setFilename(String fname) {filename = fname;}
void colorGenerator::displayWindow(String winName)
{
  if(!pattern.empty())
      imshow( winName, pattern );
  else
  {
    cout << "The object's pattern is empty" << endl;
    cout << "It will not be displayed" << endl;
    cout << "Generate the pattern using generatePattern()" << endl;
  }	
}

Mat colorGenerator::generateVerticalPattern()
{
	//destroy old pattern
	pattern = Mat::zeros( height, width, CV_8UC3 );
        srand (time(NULL));
	int pixels_filled = 0;
	int  R, G, B;
	while(pixels_filled < width)
	{
	  if((width - pixels_filled) < BLACK_SPACE)
	  {
	      pixels_filled += width - pixels_filled;
	      break;
	  } 
	  else 
	  {
	      pixels_filled += BLACK_SPACE;
	  }
	  R = rand() % 156 + 100;
	  G = rand() % 156 + 100;
	  B = rand() % 156 + 100;
	  if((width - pixels_filled) < COLOR_BAR_WIDTH)
	  {
	      MyLine(pattern, pixels_filled, true, Scalar(R,G,B), width - pixels_filled);
	      break;
	  } 
	  else
	      MyLine(pattern, pixels_filled, true, Scalar(R,G,B), COLOR_BAR_WIDTH);

	}
	return pattern;
}

Mat colorGenerator::generateHorizontalPattern()
{
	pattern = Mat::zeros(height, width, CV_8UC3);
	srand(time(NULL));
	int pixels_filled = 0;
	int R,G,B;
	while(pixels_filled < height)
	{
	  if((height - pixels_filled) < BLACK_SPACE)
	  {
	      pixels_filled += height - pixels_filled;
	      break;
	  }
	  else
	  {
	      pixels_filled += BLACK_SPACE;
	  }
	  R = rand() % 156 + 100;
	  G = rand() % 156 + 100;
	  B = rand() % 156 + 100;
	if((height - pixels_filled) < COLOR_BAR_WIDTH)
	  {
	      MyLine(pattern, pixels_filled, false, Scalar(R,G,B), height- pixels_filled);
	      break;
	  } 
	  else
	      MyLine(pattern, pixels_filled, false, Scalar(R,G,B), COLOR_BAR_WIDTH);

	}
	return pattern;
}
