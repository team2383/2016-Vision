#include <iostream>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cvFreenect.hpp"
#include "libfreenect.hpp"
#include "NetworkTable.h"

using namespace cv;
using namespace std;

double normalize360(double angle) {
    while(angle >= 360.0) {
        angle -= 360.0;    
    }
    while(angle < 0.0) {
        angle += 360.0;    
    }
    return angle;
}

int main(int argc, char **argv) {
  Mat videoMat(Size(640,480),CV_8UC3,Scalar(0));

  Freenect::Freenect freenect;
  cvFreenectDevice& device = freenect.createDevice<cvFreenectDevice>(0);

  device.setVideoFormat(FREENECT_VIDEO_IR_8BIT,   FREENECT_RESOLUTION_MEDIUM);
  device.setDepthFormat(FREENECT_DEPTH_11BIT, FREENECT_RESOLUTION_MEDIUM);

  device.startVideo();

  NetworkTable::SetClientMode();
  NetworkTable::SetTeam(2383);

  NetworkTable* table = NetworkTable::GetTable("vision");

  namedWindow("contours", CV_WINDOW_AUTOSIZE);
  while (true) {
    vector<vector<Point> > contours;
    vector<vector<Point> > filteredContours;
    vector<Vec4i> hierarchy;
    vector<Rect> ir_rects;
    vector<double> distances;
    vector<double> azimuths;
    //dont bother processing the mat if image is stale!
    if(!device.getVideo(videoMat)) continue;

    Mat gray, ranged, processed;
    cvtColor(videoMat, gray, CV_BGR2GRAY);
    inRange(gray, Scalar(64), Scalar(255), processed);

    Mat tmp = processed.clone();

    findContours(tmp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

    int i;
    for (i = 0; i < contours.size(); i++) {
      vector<Point> contour = contours[i];
      Rect r = boundingRect(contour);

      double area = contourArea(contour);
      vector<Point> hull;
      convexHull(contour, hull);
      double solidity = 100 * area / contourArea(hull);

      if (area > 400.0 && r.height > 25 && r.width > 25 && solidity < 75.0) {
        filteredContours.push_back(contour);
        ir_rects.push_back(r);
      }
    }

    /*
    Mat drawing;
    cvtColor(processed, drawing, CV_GRAY2BGR);
    */

    for (Rect r : ir_rects) {
        double x, targetX, distance, azimuth;
        x = r.br().x + r.width/2;
        x = (x - processed.size().width/2) / (processed.size().width/2);
        /* target to floor: 73
         * camera lens height: 2.5
         * camera angle from floor: 20 degrees
         * horizontal FOV: 58.5 degrees
         * vertical FOV:   46.6 degrees
         */

        distance = processed.size().height * 12.0 / (2 * r.height * tan(46.6 * M_PI / 360));
        targetX = r.tl().x + r.width / 2;
        targetX = 2 * (targetX / processed.size().width) - 1;
        azimuth = normalize360(targetX*58.5 / 2.0);

        distances.push_back(distance);
        azimuths.push_back(azimuth);

        /*
        rectangle(drawing, r.tl(), r.br(), Scalar(79,10,255), 2, 8, 0);

        Point center = Point(r.br().x - r.width / 2 - 15, r.br().y - r.height / 2);
        Point centert = Point(center.x, 10.0+center.y+r.height/2);
        Point centerb = Point(center.x, 10.0+center.y-r.height/2);

        char capTop[100];
        char capBottom[100];
        snprintf(capTop, sizeof(capTop), "x: %f azimuth: %f", x, azimuth);
        snprintf(capBottom, sizeof(capBottom), "distance: %f", distance);

        putText(drawing, capTop, centert, FONT_HERSHEY_PLAIN, 1, Scalar(255,10,79));
        putText(drawing, capBottom, centerb, FONT_HERSHEY_PLAIN, 1, Scalar(255,10,79));
        */
    }

    table->PutNumberArray("distances", distances);
    table->PutNumberArray("azimuths", azimuths);
  }
  device.stopVideo();
  device.stopDepth();
  return 0;
}
