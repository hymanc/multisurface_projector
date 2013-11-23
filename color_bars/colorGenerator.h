#ifndef  _COLOR_GEN_H
#define  _COLOR_GEN_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>
#include <iostream>

#define BLACK_SPACE     8
#define COLOR_BAR_WIDTH 2

using namespace cv;
using namespace std;

class colorGenerator{

	private:

	int width;
	int height;
	String filename;
	Mat pattern;

	void MyLine( Mat img, int x, Scalar color, int thickness);


	public:

	colorGenerator(int w, int h, String fname);
	colorGenerator(int w, int h);
	void saveToFile();
	void setWidth(int w);
	void setHeigth(int h);
	void setFilename(String fname);
	void displayWindow(String winName);
	Mat generatePattern();
};
#endif
