#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cvFreenect.hpp"
#include "libfreenect.hpp"
#include "MJPGWriter.cpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
  Mat videoMat(Size(640,480),CV_8UC1,Scalar(0));

  Freenect::Freenect freenect;
  cvFreenectDevice& device = freenect.createDevice<cvFreenectDevice>(0);

  device.  setVideoFormat(FREENECT_VIDEO_IR_8BIT,   FREENECT_RESOLUTION_MEDIUM);
  device.setDepthFormat(FREENECT_DEPTH_11BIT, FREENECT_RESOLUTION_MEDIUM);

  device.startVideo();
  device.startDepth();

  //MJPGWriter camServer(5092);

  namedWindow("contours", CV_WINDOW_AUTOSIZE);
  while (true) {
    vector<vector<Point> > contours;
    vector<vector<Point> > filteredContours;
    vector<Vec4i> hierarchy;
    vector<Rect> ir_rects;
    device.getVideo(videoMat);

    Mat original = videoMat.clone();
    Mat tmp;

    cvtColor(videoMat, tmp, CV_BGR2HLS);
    inRange(tmp, Scalar(0,16,0), Scalar(255,255,255), tmp);

    Mat temp_contours = tmp.clone();



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
      Scalar color = Scalar(128,128,128);
      drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }

    //camServer.write(drawing);
    imshow("contours", drawing);

    cvWaitKey(5);
  }
  device.stopVideo();
  device.stopDepth();
  return 0;
}
