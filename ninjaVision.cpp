#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cvFreenect.hpp"
#include "libfreenect.hpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
  bool die(false);
  string filename("snapshot");
  string suffix(".png");
  int i_snap(0),iter(0);
  Mat depthMat(Size(640,480),CV_16UC1);
  Mat depthf (Size(640,480),CV_8UC1);
  Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
  Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));

  Freenect::Freenect freenect;
  cvFreenectDevice& device = freenect.createDevice<cvFreenectDevice>(0);

	namedWindow("drawing",CV_WINDOW_AUTOSIZE);
	device.startVideo();
	device.startDepth();
	while (!die) {
		device.getDepth(depthMat);
    Mat original = video.clone();
    Mat tmp;
    cvtColor(video, tmp, CV_RGB2HLS);
    inRange(tmp, hsl_low, hsl_high, tmp);

    Mat temp_contours = tmp.clone();
    
    vector<vector<Point> > contours;
    vector<vector<Point> > filteredContours;

    findContours(temp_contours, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

    int i;
    for (i = 0; i < contours.size(); i++) {
        vector<Point> contour = contours[i];
        Rect r = boundingRect(contour);
       
        double area = contourArea(contour);
        vector<Point> hull;
        convexHull(contour, hull);
        double solidity = 100 * area / contourArea(hull);
        
        if (area > 300.0 && solidity < 75.0) {
            filteredContours.push_back(contour);
            ir_rects.push_back(r);
        }
    }   

    Mat drawing = Mat::zeros( original.size(), CV_8UC3 );
    for( int i = 0; i< filteredContours.size(); i++ )
    {
      Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
      drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }

		imshow("drawing",drawing);
		cvWaitKey(5);
	}
  device.stopVideo();
  device.stopDepth();
  return 0;
}
