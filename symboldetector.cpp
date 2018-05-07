// This code was adapted from the tutorial on cascade classifiers from the opencv documentation. It consists of a cascade classifier
// that detects circuit symbols in a hand drawn sketch.

//TODO
// - solve rotation problems --> train another cascade for 90 degree components
// - repeat for all components
// - solve problem of multiple component vectors: make every component inherit from a component class. Each one has a different
//   function that initializes its name according to its type (i.e. C for caps, R for res etc.)

// refactoring:
// - remove everything in main that isn't main program flow, and put it in functions


#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "circuit.h"
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;

//function declarations
void detectAndDisplay(Mat frame);
void loadCascades();
void setupCascades();
void findConnections(vector<Rect> components);
bool contourTouchesRect(vector<Point> & cont, Rect & rect);
bool buildCircuit(vector<vector<Point> > & contours, vector<Rect> components, Circuit & circ);

// global variables
vector<CascadeClassifier> cascades;
vector<string> cascade_names;
vector<Rect> componentBoxes;
vector<string> componentTypesList;
Mat src;
Circuit circ;

String window_name = "Component detector";

// image thresholding parameters
int param1 = 255;
int param2 = 7;

RNG rng(12345);

int main( int argc, const char** argv )
{
    cascade_names.push_back("classifier/resistor_cascade/cascade.xml");

    cout<<endl;
    cout<<"Welcome to Tarek's Sketch-to-Spice application!"<<endl;
    cout<<"* * * * * * * * * * * * * * * * * * * * * * * *"<<endl;

    while (true) {
      // first get the action choice
      cout<<endl;
      cout<<"Please enter a command:"<<endl;
      cout<<"R: Run"<<endl;
      cout<<"H: Help"<<endl;
      cout<<"O: Options"<<endl;
      cout<<"Q: Quit"<<endl;
      char c;
      cin>>c;

      if (c == 'Q' || c == 'q')
      {
        cout<<"Goodbye!"<<endl;
        break;
      }
      if (c == 'r' || c == 'R')
      {
        string str;
        cout<<"Enter a path to your image"<<endl;
        cin>>str;

        //-- 1. Load the classifiers
        loadCascades();

        //-- 2. Read the input image and resize
        src = imread(str, 1);
        if( src.empty() )
            printf(" --(!) No captured frame -- Break!");

        Size imageSize(1280, 768);
        resize(src, src, imageSize, 0, 0, INTER_NEAREST);

        //-- 3. Apply the classifiers to the image
        detectAndDisplay(src);
        waitKey(0);

      }
      else if (c == 'h' || c == 'H')
      {
        cout<<endl;
        cout<<"Sketch-to-Spice is a program that converts hand-drawn"<<endl;
        cout<<"sketches of simple circuits into Spice simulations. "<<endl;
        cout<<"Select Run to input a path to your image, and StS converts"<<endl;
        cout<<"it to a netlist, simulates it with WinSpice3 and displays the"<<endl;
        cout<<"results. You can train your own classifier if you would like"<<endl;
        cout<<"to enable StS to recognize an additional component. Use the"<<endl;
        cout<<"Options command for instructions on how to do that."<<endl;
      }
      else if (c == 'o' || c == 'O')
      {
        cout<<"Please enter a command:"<<endl;
        cout<<"T: Train new Classifier"<<endl;
        cout<<"B: Back"<<endl;
        string str;
        cin>>str;
      }
      else
      {
        cerr << "Wrong input!"<< endl;
      }
    }

    return 0;
}

//
void detectAndDisplay( Mat frame )
{
  cvtColor( frame, frame, COLOR_RGB2GRAY );
  blur( frame, frame, Size(3, 3) );
  adaptiveThreshold(frame, frame, param1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 55, param2);
  blur( frame, frame, Size(3, 3) );

  string name;

  for (size_t i = 0; i < cascades.size(); i++)
  {
    Scalar color = Scalar( 90, 90, 90);
    vector<Rect> components;
    name = cascade_names[i].erase(0, 11);
    name.erase(3);

    //-- Detect components
    cout<<cascades[i].empty()<<"\n";
    cascades[i].detectMultiScale( frame, components, 3, 3, 0, Size(5, 800));

    //-- draw rectangles around components and add them to the list of all components (componentBoxes)
    for ( size_t i = 0; i < components.size(); i++ )
    {
      rectangle(frame, components[i].tl(), components[i].br(), color, 2, 8, 0 );
      componentTypesList.push_back(name);
      componentBoxes.push_back(components[i]);
    }
  }

  //-- Show what you got
  imshow( window_name, frame );
}

// loads the xml files containing the trained cascade classifiers
void loadCascades()
{
  for (size_t i = 0; i < cascade_names.size(); i++)
  {
    CascadeClassifier c;
    if( !c.load(cascade_names[i])) {printf("--(!)Error loading resistor cascade\n");};
    cascades.push_back(c);
  }
}

// removes the components from the image, finds the connections between them,
// builds a circuit and outputs the netlist to a text file
void findConnections(vector<Rect> components)
{
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  threshold(src, src, 250, 255, THRESH_BINARY);

  // remove components from image to leave only connections
  Scalar white = Scalar(255, 255, 255);
  Mat roi;
  for(size_t i = 0; i < components.size(); i++)
  {
    roi = src(components[i]);
    roi.setTo(white);
  }

  // find the contours of the connections
  findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
  vector<vector<Point> > contours_poly(contours.size());

  // build the circuit
  buildCircuit(contours, components, circ);

  // print the netlist to a file called netlist.txt
  circ.printNetlist();

  // draw the contours
  Mat drawing = Mat::zeros(src.size(), CV_8UC3);

  for(size_t i = 0; i < contours.size(); i++)
  {
    approxPolyDP(contours[i], contours_poly[i], 3, true );
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
    drawContours(src, contours_poly, (int)i, color, 2, 8, vector<Vec4i>(), 0, Point());
  }

  namedWindow("Contours", WINDOW_AUTOSIZE);
  imshow("Contours", src);
}

// loops through all points in a given contour and checks if one of them is inside rect
bool contourTouchesRect(vector<Point> & cont, Rect & rect)
{
  for( size_t i = 0; i < cont.size(); i++)
  {
    if(rect.contains(cont[i]))
      return true;
  }
  return false;
}

// a for loop iterates over all rectangles, and for each one, another loop iterates over all valid contours (connections).
// When a rectangle is touched by two distinct contours, it is added to the circuit class.
bool buildCircuit(vector<vector<Point> > & contours, vector<Rect> components, Circuit & circuit)
{
  for(size_t i = 0; i < components.size(); i++)
  {
    Component comp;
    //c.initializeName(i);
    comp.name = "C" + to_string(i);
    comp.value = 10;

    bool sig = 0;
    for(size_t j = 0; j < contours.size(); j++)
    {
      if(contourTouchesRect(contours[j], components[i]))
      {
        if(sig == 0)
        {
          comp.rightNode = j;
          sig = 1;
        }
        else
        {
          comp.leftNode = j;
          circuit.addComponent(comp);
          break;
        }
      }
    }
  }
  return true;   // use later to catch errors
}
