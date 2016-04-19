#include <iostream>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cvFreenect.hpp"
#include "libfreenect.hpp"
#include "NetworkTable.h"

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
  Mat videoMat(Size(640,480),CV_8UC3,Scalar(0));

  Freenect::Freenect freenect;
  cvFreenectDevice& device = freenect.createDevice<cvFreenectDevice>(0);

  device.setVideoFormat(FREENECT_VIDEO_IR_8BIT,   FREENECT_RESOLUTION_MEDIUM);
  device.setDepthFormat(FREENECT_DEPTH_11BIT, FREENECT_RESOLUTION_MEDIUM);

  device.startVideo();

  NetworkTable::SetClientMode();
  NetworkTable::SetIPAddress("10.23.83.2");

  shared_ptr<NetworkTable> table = NetworkTable::GetTable("vision");

  //namedWindow("contours", CV_WINDOW_AUTOSIZE);
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
    //180-255 at SFL & ROC
    inRange(gray, Scalar(160), Scalar(255), processed);

    Mat tmp = processed.clone();

    findContours(tmp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

    int i;
    for (i = 0; i < contours.size(); i++) {
      vector<Point> contour = contours[i];
      Rect r = boundingRect(contour);

      double area = contourArea(contour);
      vector<Point> hull;
      convexHull(contour, hull);
      double aspect = (double) r.width / (double) r.height;
      double solidity = 100.0 * area / contourArea(hull);

      if (aspect > 1.0 && r.width > 30 && solidity < 50.0) {
        filteredContours.push_back(contour);
        ir_rects.push_back(r);
      }
    }

    Mat drawing;
    cvtColor(processed, drawing, CV_GRAY2BGR);

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

        distance = ((processed.size().width*(10.0/r.width)))/(tan(46.6 * M_PI/180.0)/2.0);
        targetX = r.tl().x + r.width / 2;
        targetX = 2 * (targetX / processed.size().width) - 1;
        azimuth = targetX*58.5 / 2.0;

        distances.push_back(distance);
        azimuths.push_back(azimuth);

        rectangle(drawing, r.tl(), r.br(), Scalar(79,10,255), 2, 8, 0);

        Point center = Point(r.br().x - r.width / 2 - 15, r.br().y - r.height / 2);
        Point centert = Point(center.x, 10.0+center.y+r.height/2);
        Point centerb = Point(center.x, 10.0+center.y-r.height/2);

        char capTop[100];
        char capBottom[100];
        snprintf(capTop, sizeof(capTop), "asp: %f w: %d", (double) r.width/(double) r.height, r.width);
        snprintf(capBottom, sizeof(capBottom), "h: %d, az: %f, dist: %f", r.height, azimuth, distance);
        //snprintf(capTop, sizeof(capTop), "x: %f azimuth: %f", x, azimuth);
        //snprintf(capBottom, sizeof(capBottom), "distance: %f", distance);

        putText(drawing, capTop, centert, FONT_HERSHEY_PLAIN, 1, Scalar(255,10,79));
        putText(drawing, capBottom, centerb, FONT_HERSHEY_PLAIN, 1, Scalar(255,10,79));
    }

    table->PutNumber("watchdog_result", rand());
    table->PutNumberArray("distances", distances);
    table->PutNumberArray("azimuths", azimuths);

    //imwrite("snap.jpg", gray);
    //imwrite("proc.jpg", drawing);
  }
  device.stopVideo();
  device.stopDepth();
  return 0;
}
