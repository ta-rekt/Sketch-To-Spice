#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "circuit.h"
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;
//


// detects symbols and
void detectAndDisplay(Mat frame,
  vector<CascadeClassifier> & cascades,
  vector<string> & cascade_names,
  vector<Rect> & component_boxes,
  vector<string> & component_names)
{
  cvtColor( frame, frame, COLOR_RGB2GRAY );
  blur( frame, frame, Size(3, 3) );
  adaptiveThreshold(frame, frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 55, 7);
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
      component_names.push_back(name);
      component_boxes.push_back(components[i]);
    }
  }

  //-- Show what you got
  imshow( "Component zetector", frame );
}

// loads the xml files containing the trained cascade classifiers
void loadCascades(vector<CascadeClassifier> & cascades, vector<string> & cascade_names)
{
  for (size_t i = 0; i < cascade_names.size(); i++)
  {
    CascadeClassifier c;
    if( !c.load(cascade_names[i])) {printf("--(!)Error loading resistor cascade\n");};
    cascades.push_back(c);
  }
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
void buildCircuit(vector<vector<Point> > & contours, vector<Rect> component_boxes, vector<string> & component_names, Circuit & circuit)
{
  for(size_t i = 0; i < component_boxes.size(); i++)
  {
    Component comp;

    char compFirstLetter = component_names[i][0];
    compFirstLetter = (char)toupper(compFirstLetter);

    comp.name = compFirstLetter + to_string(i);
    comp.value = 10;

    bool sig = 0;
    for(size_t j = 0; j < contours.size(); j++)
    {
      if(contourTouchesRect(contours[j], component_boxes[i]))
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
}

// removes the components from the image, finds the connections between them,
// builds a circuit and outputs the netlist to a text file
void findConnections(Mat src, vector<Rect> component_boxes, vector<string> & component_names)
{
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  Circuit circ;
  RNG rng(12345);

  Size imageSize(1280, 768);
  resize(src, src, imageSize, 0, 0, INTER_NEAREST);
  cvtColor( src, src, COLOR_RGB2GRAY );
  blur( src, src, Size(3,3) );
  adaptiveThreshold(src, src, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 55, 7);
  threshold(src, src, 250, 255, THRESH_BINARY);

  // remove components from image to leave only connections
  Scalar white = Scalar(255, 255, 255);
  Mat roi;
  for(size_t i = 0; i < component_boxes.size(); i++)
  {
    roi = src(component_boxes[i]);
    roi.setTo(white);
  }

  // find the contours of the connections
  findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
  vector<vector<Point> > contours_poly(contours.size());

  // build the circuit
  buildCircuit(contours, component_boxes, component_names, circ);

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
}
