// This component detects connections between circuit symbols. It takes an image of a hand-drawn circuit diagram and a vector of
// rectangles that represent the locations of components output by the symbol detector. It then decides if two rectanlges are joined
// together by a line using contours detection.


#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "circuit.h"
#include <iostream>
using namespace cv;
using namespace std;

Mat src;
string arg2;
Circuit circ;

int thresh = 250;        //magic number
int max_thresh = 255;
int param1 = 255;
int param2 = 10;
RNG rng(12345);
void findConnections(vector<Rect> components);
bool contourTouchesRect(vector<Point> & cont, Rect & rect);
bool buildCircuit(vector<vector<Point> > & contours, vector<Rect> components, Circuit & circ);


int main( int argc, char** argv )
{
  CommandLineParser parser( argc, argv, "{@input | ../data/stuff.jpg | input image}" );
  src = imread( parser.get<String>( "@input" ), IMREAD_COLOR );

  if( src.empty() )
  {
    cout << "Could not open or find the image!\n" << endl;
    cout << "usage: " << argv[0] << " <Input image>" << endl;
    return -1;
  }

  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  vector<vector<Point> > contours_poly(contours.size());

  Size imageSize(600, 600);
  resize(src, src, imageSize, 0, 0, INTER_NEAREST);

  //add a filtering stage to make the image b&w high contrast and flat --> adaptiveThreshold
  cvtColor( src, src, COLOR_RGB2GRAY );
  blur( src, src, Size(3,3) );
  adaptiveThreshold(src, src, param1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 55, param2);

  imshow( "source_window", src );

  const char* source_window = "Source";
  namedWindow( source_window, WINDOW_AUTOSIZE );

  findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
  for(size_t i = 0; i < contours.size(); i++)
  {
    approxPolyDP(contours[i], contours_poly[i], 3, true );
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
    drawContours(src, contours_poly, (int)i, color, 2, 8, vector<Vec4i>(), 0, Point());
  }
  // findConnections(components);

  waitKey(0);
  return(0);
}

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
    Component c;
    //c.initializeName(i);
    c.name = "C" + to_string(i);
    c.value = 10;

    bool sig = 0;
    for(size_t j = 0; j < contours.size(); j++)
    {
      if(contourTouchesRect(contours[j], components[i]))
      {
        if(sig == 0)
        {
          c.rightNode = j;
          sig = 1;
        }
        else
        {
          c.leftNode = j;
          circuit.addComponent(c);
          break;
        }
      }
    }
  }
  return true;   // use later to catch errors
}
