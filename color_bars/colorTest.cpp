#include "colorGenerator.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>

#define BLACK_SPACE     8
#define COLOR_BAR_WIDTH 2

int w;
int h;

using namespace cv;

void MyLine( Mat img, int x, Scalar color, int thickness);

int main(int argc, char* argv[]){
    
    /* Args - Not yet implemented
        1. Height in pixels
     
        2. Width in pixels
     
        3. location
     */
    
//    if(argc != 4){
//        printf("Incorrect number of arguments");
//        return -1;
//    }
//    
//    h = atoi(argv[1]);
//    w = atoi(argv[2]);
//    char* filename = argv[3];
    
    h = 480;
    w = 640;
    String filename = "patternV.png";
    colorGenerator *g = new colorGenerator(w,h,filename);
    g->generateVerticalPattern();
    g->displayWindow("Pattern");
    g->saveToFile();
    
    waitKey( 0 );
    filename = "patternH.png";
    g->generateHorizontalPattern();
    g->displayWindow("Pattern2");
    g->setFilename(filename);
    g->saveToFile();
    waitKey( 0 );
    return(0);
}

