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
    
    //h = 750;
    //w = 1000;
    w = 640;
    h = 480;
    String filename = "/User/scottmeldrum/Pictures/pattern.png";
    
    srand (time(NULL));
    
    /// Windows names
    char window[] = "Color Pattern";
    
    /// Create black empty images
    Mat pattern = Mat::zeros( h, w, CV_8UC3 );
    
    int pixels_filled = 0;
    
    int  R, G, B;
    
    while(pixels_filled < w){
        if((w - pixels_filled) < BLACK_SPACE){
            pixels_filled += w - pixels_filled;
            break;
        } else {
            pixels_filled += BLACK_SPACE;
        }

        R = rand() % 206 + 50;
        G = rand() % 206 + 50;
        B = rand() % 206 + 50;
        
        if((w - pixels_filled) < COLOR_BAR_WIDTH){
            MyLine(pattern, pixels_filled, Scalar(R,G,B), w - pixels_filled);
            break;
        } else {
            MyLine(pattern, pixels_filled, Scalar(R,G,B), COLOR_BAR_WIDTH);
        }
        
    }
    imwrite( filename, pattern );
    imshow( window, pattern );
    
    waitKey( 0 );
    return(0);
}

void MyLine( Mat img, int x, Scalar color, int thickness)
{
//    int thickness = 2;
    int lineType = 8;
    line( img,
         Point(x,0),
         Point(x,h),
         color,
         thickness,
         lineType );
}
