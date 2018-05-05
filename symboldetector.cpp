// This code was adapted from the tutorial on cascade classifiers from the opencv documentation. It consists of a cascade classifier
// that detects circuit symbols in a hand drawn sketch.

//TODO
// - solve rotation problems --> train another cascade for 90 degree components
// - repeat for all components

// next step:
// - output locations of detected components (already does that)
// - work on detecting lines, connections
// - output connections to text file
// - create netlist from text file, or directly, and finish

// refactoring:
// - remove everything in main that isn't main program flow, and put it in functions


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

int param1 = 255;
int param2 = 10;

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
    Size imageSize(1280, 768);
    resize(src, src, imageSize, 0, 0, INTER_NEAREST);
    detectAndDisplay( src );

    waitKey(0);

    return 0;
}

void detectAndDisplay( Mat frame )
{
    std::vector<Rect> resistors;

    cvtColor( frame, frame, COLOR_RGB2GRAY );        //here's the problem
    blur( frame, frame, Size(3, 3) );
    adaptiveThreshold(frame, frame, param1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 55, param2);

    //-- Detect faces
    cout<<resistor_cascade.empty()<<"\n";
    resistor_cascade.detectMultiScale( frame, resistors, 3, 3, 0, Size(5, 700));

    for ( size_t i = 0; i < resistors.size(); i++ )
    {
        Point center( resistors[i].x + resistors[i].width/2, resistors[i].y + resistors[i].height/2 );
        ellipse( frame, center, Size( resistors[i].width/2, resistors[i].height/2 ), 0, 0, 360, Scalar( 120, 120, 120 ), 4, 8, 0 );
    }
    //-- Show what you got
    imshow( window_name, frame );
}
