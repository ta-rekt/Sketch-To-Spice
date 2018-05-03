#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;



//TODO
// - rescale the extracted images to they all fit 100 * 25
// - generate more training data using createsamples
// - solve scale and rotation problems
// - repeat for all components

// next step:
// - output locations of detected components
// - work on detecting lines, connections
// - output connections to text file
// - create netlist from text file, or directly, and finish


void detectAndDisplay( Mat frame );
String resistor_cascade_name;
CascadeClassifier resistor_cascade;
String window_name = "Resistor detector";

int param1 = 255;
int param2 = 35;

int main( int argc, const char** argv )
{
    CommandLineParser parser(argc, argv,
        "{help h||}"
        "{resistor_cascade|/Users/mbpr/Desktop/AUB/2017-2018/2017-2018Spring/EECE_437/Individual_Project/EECE-437-Individual-Project/classifier/resistor_cascade/cascade.xml|}");

    parser.about( "\nThis program detects resistor symbols in a circuit sketch\n");
    parser.printMessage();

    resistor_cascade_name = parser.get<String>("resistor_cascade");
    Mat src;

    //-- 1. Load the cascades
    if( !resistor_cascade.load(resistor_cascade_name) ){ printf("--(!)Error loading resistor cascade\n"); return -1; };

    //-- 2. Read the input image
    src  = imread(argv[1], 1);
    if( src.empty() )
        printf(" --(!) No captured frame -- Break!");

    //-- 3. Apply the classifier to the image
    detectAndDisplay( src );
    //imshow( window_name, frame );
    waitKey(0);

    return 0;
}

void detectAndDisplay( Mat frame )
{
    std::vector<Rect> resistors;
    Mat frame_gray;
    Mat frame_flat;

    cvtColor( frame, frame_gray, COLOR_RGB2GRAY );        //here's the problem

    equalizeHist( frame_gray, frame_gray );

    adaptiveThreshold(frame_gray, frame_flat, param1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, param2);
    //-- Detect faces
    cout<<resistor_cascade.empty()<<"\n";
    resistor_cascade.detectMultiScale( frame_flat, resistors, 3, 3, 0, Size(10, 200));

    for ( size_t i = 0; i < resistors.size(); i++ )
    {
        Point center( resistors[i].x + resistors[i].width/2, resistors[i].y + resistors[i].height/2 );
        ellipse( frame_flat, center, Size( resistors[i].width/2, resistors[i].height/2 ), 0, 0, 360, Scalar( 120, 120, 120 ), 4, 8, 0 );
    }
    //-- Show what you got
    imshow( window_name, frame_flat );
}
