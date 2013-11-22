#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>

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
    
    h = 750;
    w = 1000;
    String filename = "/User/scottmeldrum/Pictures/pattern.bmp";
    
    srand (time(NULL));
    
    /// Windows names
    char window[] = "Color Pattern";
    
    /// Create black empty images
    Mat pattern = Mat::zeros( h, w, CV_8UC3 );
    
    int pixels_filled = 0;
    
    int color_width, R, G, B;
    
    while(pixels_filled < w){
        R = rand() % 156 + 100;
        G = rand() % 156 + 100;
        B = rand() % 156 + 100;
        
        if((w - pixels_filled) < 10){
            color_width = w - pixels_filled;
            MyLine(pattern, pixels_filled, Scalar(R,G,B), color_width);
            break;
        } else {
            color_width = rand() % 5 + 5;
            MyLine(pattern, pixels_filled, Scalar(R,G,B), color_width);
        }
        
        if((w - pixels_filled) < 25){
            pixels_filled += w - pixels_filled;
            break;
        } else {
            pixels_filled += rand() % 5 + 20;
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
