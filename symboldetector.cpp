#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;
void detectAndDisplay( Mat frame );
String resistor_cascade_name;
CascadeClassifier resistor_cascade;
String window_name = "Resistor detector";
int main( int argc, const char** argv )
{
    CommandLineParser parser(argc, argv,
        "{help h||}"
        "{resistor_cascade|/Users/mbpr/Desktop/AUB/2017-2018/2017-2018Spring/EECE_437/Individual_Project/resistor_cascade/cascade.xml|}");
    parser.about( "\nThis program detects resistor symbols in a circuit sketch\n");
    parser.printMessage();
    resistor_cascade_name = parser.get<String>("resistor_cascade");
    Mat frame;

    //-- 1. Load the cascades
    if( !resistor_cascade.load("/Users/mbpr/Desktop/AUB/2017-2018/2017-2018Spring/EECE_437/Individual_Project/resistor_cascade/cascade.xml") ){ printf("--(!)Error loading resistor cascade\n"); return -1; };

    //-- 2. Read the input image

    // capture.open( 0 );
    // if ( ! capture.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }
    // while ( capture.read(frame) )
    // {
    //     if( frame.empty() )
    //     {
    //         printf(" --(!) No captured frame -- Break!");
    //         break;
    //     }
    //     //-- 3. Apply the classifier to the frame
    //     detectAndDisplay( frame );
    //     if( waitKey(10) == 27 ) { break; } // escape
    // }

    //-- 3. Apply the classifier to the image

    frame  = imread(argv[1], IMREAD_COLOR);
    if( frame.empty() )
    {
        printf(" --(!) No captured frame -- Break!");
    }
    detectAndDisplay( frame );
    //imshow( window_name, frame );
    waitKey(0);

    return 0;
}
void detectAndDisplay( Mat frame )
{
    std::vector<Rect> resistors;
    Mat frame_gray;
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    //-- Detect faces

    resistor_cascade.detectMultiScale( frame_gray, resistors, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(28, 13));

    for ( size_t i = 0; i < resistors.size(); i++ )
    {
        Point center( resistors[i].x + resistors[i].width/2, resistors[i].y + resistors[i].height/2 );
        ellipse( frame, center, Size( resistors[i].width/2, resistors[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        // Mat faceROI = frame_gray( resistors[i] );
        // std::vector<Rect> eyes;
        // //-- In each face, detect eyes
        // eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );
        // for ( size_t j = 0; j < eyes.size(); j++ )
        // {
        //     Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
        //     int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
        //     circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
        // }
    }
    //-- Show what you got
    imshow( window_name, frame );
}
