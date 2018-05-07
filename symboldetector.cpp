// This code was adapted from the tutorial on cascade classifiers from the opencv documentation. It consists of a cascade
// classifier that detects circuit symbols in a hand drawn sketch.


#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "functions.h"
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;


int main( int argc, const char** argv )
{
    vector<CascadeClassifier> cascades;
    vector<string> cascade_names;
    vector<Rect> component_boxes;
    vector<string> component_names;
    Mat src;

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
        loadCascades(cascades, cascade_names);

        //-- 2. Read the input image and resize
        src = imread(str, 1);
        if( src.empty() )
        {
          cout << "Could not open or find the image!\n" << endl;
          cout << "usage: " << str << " <Input image>" << endl;
          return -1;
        }
        Size imageSize(1280, 768);
        resize(src, src, imageSize, 0, 0, INTER_NEAREST);

        //-- 3. Detect the circuit symbols
        detectAndDisplay(src, cascades, cascade_names, component_boxes, component_names);

        //-- 4. Find the connections between each component
        findConnections(src, component_boxes, component_names);

        waitKey(0);

      }
      else if (c == 'h' || c == 'H')
      {
        cout<<endl;
        cout<<"Sketch-to-Spice is a program that converts hand-drawn"<<endl;
        cout<<"sketches of simple DC circuits into Spice simulations. "<<endl;
        cout<<"Select Run to input a path to your image, and StS converts"<<endl;
        cout<<"it to a netlist. You can train your own classifier if you would like"<<endl;
        cout<<"to enable StS to recognize an additional component. Use the"<<endl;
        cout<<"Options command for instructions on how to do that."<<endl;
        cout<<"WARNING: This is a trial version that can only detect resistors."<<endl;
      }
      else if (c == 'o' || c == 'O')
      {
        cout<<"Please enter a command:"<<endl;
        cout<<"T: Train new Classifier"<<endl;
        cout<<"B: Back"<<endl;
        char s;
        cin>>s;
        if (s == 'T' || s == 't')
        {
          cout<<"Enter path of classifier xml file. It should be contained in a folder"<<endl;
          cout<<"called classifier inside the working directory."<<endl;
          cout<<"Example: classifier/resistor_classifier/cascade.xml"<<endl;
          string str2;
          cin>>str2;
          cascade_names.push_back(str2);
        }

      }
      else
      {
        cerr << "Wrong input!"<< endl;
      }
    }
    return 0;
}
