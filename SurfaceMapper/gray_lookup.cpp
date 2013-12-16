#include "gray_lookup.hpp"

using namespace cv;
using namespace std;

/*
 Returns an array of values, indexed by gray value holding which column it belongs to.
 A row value of 0 indicates that the desired gray value doesn't exist in projected image.
 */
int * get_column_LUT(Mat v_projected, int maxGray){
	int * column_lut = new int[maxGray];
    
    //Ensure a starting value of 0 for all entries
	for(int i = 0; i < maxGray; i++){
		column_lut[i] = 0;
	}
    
	int column = 0;
    
    //a gray value that is guaranteed to trigger indexing the first column
	int gray = ~(v_projected.at<uchar>(0,0));
    
	for(int x = 0; x < v_projected.size().width; x++){
        int val = v_projected.at<uchar>(0,x);
        if(val != gray){
            gray = val;
            column_lut[gray] = ++column;
        }
    }
    return column_lut;
}

/*
 Returns an array of values, indexed by gray value holding which row it belongs to.
 A row value of 0 indicates that the desired gray value doesn't exist in projected image.
 */
int * get_row_LUT(Mat h_projected, int maxGray){
	int * row_lut = new int[maxGray];
    
    //Ensure a starting value of 0 for all entries
	for(int i = 0; i < maxGray; i++){
		row_lut[i] = 0;
	}
    
	int row = 0;
    
    //a gray value that is guaranteed to trigger indexing the first row
	int gray = ~(h_projected.at<uchar>(0,0));

	for(int y = 0; y < h_projected.size().height; y++){
        int val = h_projected.at<uchar>(y,0);
        if(val != gray){
            gray = val;
            row_lut[gray] = ++row;
        }
    }
    
    return row_lut;
}

// int main() {
//     //This is just a sampler to show they work with the gray map that combines both dimensions
//     Mat graycode = imread("/Users/scottmeldrum/Pictures/Disparity/grayMap.png", CV_LOAD_IMAGE_GRAYSCALE);
    
//     int * rLUT = get_row_LUT(graycode);
//     int * cLUT = get_column_LUT(graycode);
    
//     for(int i = 0; i < 256; i++){
//         if(rLUT[i]!=0){
//             cout << "Gray:\t" << i << "\tRow:\t" << rLUT[i] << endl;
//         }
//     }
    
//     cout << endl << endl;
//     for(int i = 0; i < 256; i++){
//         if(cLUT[i]!=0){
//             cout << "Gray:\t" << i << "\tCol:\t" << cLUT[i] << endl;
//         }
//     }
    
//     while(waitKey(1)!=ESC_KEY);
 
//     return 0;
// }


