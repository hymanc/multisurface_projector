/**
 * @file match.cpp
 */


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

#define SPACE_KEY 32
#define ESC_KEY 27

using namespace cv;
using namespace std;

int main() {
    
//960x720 - boxes :: 60*16 x 240*3
//848x480 - gray  :: 53*16 x 240*2 -- 14*4 56
    
    // (56,0) to (904, 480)
    
//Resize gray: 60/53*col, 3/2*row
    
//    CvCapture * cap1 = cvCaptureFromCAM(0);
//    if(!cap1){
//        cout << "Error setting up cap";
//        return -1;
//    }
//    
//    cvSetCaptureProperty(cap1, CV_CAP_PROP_FRAME_HEIGHT, 480);
//    cvSetCaptureProperty(cap1, CV_CAP_PROP_FRAME_WIDTH, 620);
//    
//    Mat frame1, F1, frame2, F2;
//    
//    while(waitKey(1)!=SPACE_KEY){
//        frame1 = cvQueryFrame(cap1);
//        imshow("Frame 1", frame1);
//    }
//    F1 = frame1.clone();
//    cvtColor(F1,F1,CV_BGR2GRAY);
//    F1.convertTo(F1, CV_8UC1);
//    
//    while(waitKey(1)!=SPACE_KEY){
//        frame2 = cvQueryFrame(cap1);
//        imshow("Frame 2", frame2);
//    }
//    F2 = frame2.clone();
//    cvtColor(F2,F2,CV_BGR2GRAY);
//    F2.convertTo(F2, CV_8UC1);
    
    
    Mat B = imread("/Users/scottmeldrum/Pictures/Disparity/boxes.png", CV_LOAD_IMAGE_GRAYSCALE);
    Mat graycode = imread("/Users/scottmeldrum/Pictures/Disparity/grayMap.png", CV_LOAD_IMAGE_GRAYSCALE);
    
    Rect myROI(56, 0, 848, 480);
    Mat boxes = B(myROI);
    
    boxes.convertTo(boxes, CV_8UC1);
    
    //    Mat G = graycode.clone();
    //
    //    resize(G, graycode, boxes.size(), 0, 0, CV_INTER_CUBIC);
    
    graycode.convertTo(graycode, CV_8UC1);

    Mat F1, F2;
    F1 = boxes.clone();
    F2 = graycode.clone();
    
    
    vector<KeyPoint> key1, key2;
    Mat desc1, desc2;
    
//    FastAdjuster * fastad = new FastAdjuster(10,true);
//    FeatureDetector * det = new DynamicAdaptedFeatureDetector ( fastad, 5000, 10000, 10);
//    det->detect(F1, key1);
//    det->detect(F2, key2);
    
    SurfFeatureDetector detector(400);
    vector<KeyPoint> keypoints1, keypoints2;
    detector.detect(F1, key1);
    detector.detect(F2, key2);
    
    Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create("SIFT");
    extractor->compute( F1, key1, desc1 );
    extractor->compute( F2, key2, desc2 );
    
    vector< vector<DMatch> > matches;
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
    matcher->knnMatch( desc1, desc2, matches, 100 );
    
    //look whether the match is inside a defined area of the image
    //only 25% of maximum of possible distance
    double tresholdDist = 1 * sqrt(double(F1.size().height*F1.size().height + F1.size().width*F1.size().width));
    
    vector< DMatch > good_matches2;
    good_matches2.reserve(matches.size());
    for (size_t i = 0; i < matches.size(); ++i)
    {
        for (int j = 0; j < matches[i].size(); j++)
        {
            Point2f from = key1[matches[i][j].queryIdx].pt;
            Point2f to = key2[matches[i][j].trainIdx].pt;
            
            //calculate local distance for each possible match
            double dist = sqrt((from.x - to.x) * (from.x - to.x) + (from.y - to.y) * (from.y - to.y));
            
            //save as best match if local distance is in specified area and on same height
            if (dist < tresholdDist && abs(from.y-to.y)<5)
            {
                good_matches2.push_back(matches[i][j]);
                j = matches[i].size();
            }
        }
    }

    Mat visual;
    drawMatches(F1, key1, F2, key2, good_matches2, visual);
    
    imshow("Matches", visual);
    
    
    while(waitKey(1)!=ESC_KEY);
 
    return 0;
}


