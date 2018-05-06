// This component detects connections between circuit symbols. It takes an image of a hand-drawn circuit diagram and a vector of
// rectangles that represent the locations of components output by the symbol detector. It then decides if two rectanlges are joined
// together by a line using contours detection.

// TODO
// - find out how to get contours for circuit connections (already done that in symboldetector)
// - find out how to tell if a contour is continuous from rectangle A to rectangle B, which means it joins components A and B
// - can store all connections inside a adjacency matrix, which gets filled up incrementally. This would solve the problem of

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "circuit.h"
#include <iostream>
using namespace cv;
using namespace std;

Mat src;
string arg2;

int thresh = 250;        //magic number
int max_thresh = 255;
int param1 = 255;
int param2 = 10;
RNG rng(12345);
void thresh_callback(int, void* );
bool contourTouchesRect(vector<Point> & cont, Rect & rect);

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

  Size imageSize(600, 600);
  resize(src, src, imageSize, 0, 0, INTER_NEAREST);

  //add a filtering stage to make the image b&w high contrast and flat --> adaptiveThreshold
  cvtColor( src, src, COLOR_RGB2GRAY );
  blur( src, src, Size(3,3) );
  adaptiveThreshold(src, src, param1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 55, param2);

  imshow( "source_window", src );

  const char* source_window = "Source";
  namedWindow( source_window, WINDOW_AUTOSIZE );

  thresh_callback( 0, 0 );

  waitKey(0);
  return(0);
}

void thresh_callback(int, void* )
{
  Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  threshold( src, src, thresh, 255, THRESH_BINARY );

  //create some rectangles manually for now, later symboldetector will take care of it
  //r1
  Point r1tl(100, 30);
  Point r1br(230, 85);
  Rect R1(r1tl, r1br);

  //v1
  Point v1tl(10, 220);
  Point v1br(70, 340);
  Rect V1(v1tl, v1br);

  Scalar color2 = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );

  // rectangle( src, r1tl, r1br, color2, 2, 8, 0 );
  // rectangle( src, v1tl, v1br, color2, 2, 8, 0 );

  //delete region inside rectangles
  Mat roi = src(R1);
  roi.setTo(Scalar(255, 255, 255));
  roi = src(V1);
  roi.setTo(Scalar(255, 255, 255));

  findContours( src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );

  // find the contours and bounding boxes
  for( size_t i = 0; i < contours.size(); i++ )
  {
    approxPolyDP( contours[i], contours_poly[i], 3, true );

    if(contourTouchesRect(contours[i], R1))
      cout<<"contour touches R1"<<endl;
    if(contourTouchesRect(contours[i], V1))
      cout<<"contour touches V1"<<endl;
  }

  // draw the contours
  Mat drawing = Mat::zeros( src.size(), CV_8UC3 );

  for( size_t i = 0; i < contours.size(); i++ )
  {
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    drawContours( src, contours_poly, (int)i, color, 2, 8, vector<Vec4i>(), 0, Point() );
  }

  namedWindow( "Contours", WINDOW_AUTOSIZE );
  imshow( "Contours", src );
}


bool contourTouchesRect(vector<Point> & cont, Rect & rect)
{
  for( size_t i = 0; i < cont.size(); i++)
  {
    if(rect.contains(cont[i]))
      return true;
  }
  return false;
}
