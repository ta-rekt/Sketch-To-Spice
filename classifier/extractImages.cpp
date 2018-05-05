//  This code chops an image of many sketched symbols used for training into images that contain one symbol each. Here are the different
// steps taken:

// 1 - the image is turned to grayscale and thresholded to remove the effect of uneven lighting
// 2 - the image is blurred to facilitate detecting symbols from their contours
// 3 - the contours are found
// 4 - bounding boxes are found from the contours
// 5 - the original image is cropped according to the bounding box of each component, and the new images are rescaled and exported in a folder

//TODO
// refactoring:
// - remove everything in main that isn't main program flow, and put it in functions

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

Mat src;
Mat src_gray;
Mat src_flat;
Mat src_blur;
Mat src_crop;
Mat output;
string arg2;

int thresh = 250;        //magic number
int max_thresh = 255;
int param1 = 255;
int param2 = 8;
RNG rng(12345);
void thresh_callback(int, void* );
void setupDir(string a);
void writeToFile(string arg2, string path);

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
  arg2 = argv[2];

  //add a filtering stage to make the image b&w high contrast and flat --> adaptiveThreshold
  cvtColor( src, src_gray, COLOR_RGB2GRAY );
  adaptiveThreshold(src_gray, src_flat, param1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, param2);

  imshow( "source_window", src_flat );
  blur( src_flat, src_blur, Size(12,12) );

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
  threshold( src_blur, threshold_output, thresh, 255, THRESH_BINARY );
  findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );
  vector<Point2f>center( contours.size() );
  vector<float>radius( contours.size() );

  // find the contours and bounding boxes
  string name, path;

  for( size_t i = 0; i < contours.size(); i++ )
  {
    approxPolyDP( contours[i], contours_poly[i], 3, true );
    boundRect[i] = boundingRect( contours_poly[i] );
    minEnclosingCircle( contours_poly[i], center[i], radius[i] );
  }

  // draw the contours and bounding boxes
  Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );

  setupDir(arg2);

  for( size_t i = 0; i < contours.size(); i++ )
  {
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    drawContours( drawing, contours_poly, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
    if(norm(boundRect[i].tl() - boundRect[i].br()) > 40 && norm(boundRect[i].tl() - boundRect[i].br()) < 350)      //magic number
    {
      rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
      src_crop = src_flat(boundRect[i]);    //crop out bounding boxes from thresholded image
      Size dsize(200,200);
      resize(src_crop, output, dsize, 0, 0, INTER_NEAREST);   //rescale cropped images to 100 x 25
      name =  arg2 + to_string(i) + ".png";
      path = arg2 + "/" + name;
      imwrite(path, output);
      writeToFile(arg2, path);
    }
  }
  namedWindow( "Contours", WINDOW_AUTOSIZE );
  imshow( "Contours", drawing );
}


void writeToFile(string arg2, string path)
{
  //write path and bounding box dimensions to info file
  int r, c;
  r = output.rows - 1;
  c = output.cols - 1;
  string command = "echo " + path + "  1  " + " 0 0 " + to_string(c) + " " + to_string(r) + " >> " + arg2 + "info.txt";
  const char* writeToInfoFile = command.c_str();
  system(writeToInfoFile);

  command = "echo " + path + " >> " + arg2 + "bg.txt";
  const char* writeToBgFile = command.c_str();
  system(writeToBgFile);
}


void setupDir(string a)
{
  //set up directory and text file
  string command = "mkdir " + a;
  const char* createFolder = command.c_str();
  system(createFolder);
}
