#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cvFreenect.hpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
  Mat depthMat(Size(640,480),CV_16UC1);
  Mat depthf (Size(640,480),CV_8UC1);
  Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
  Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));

  Freenect::Freenect freenect;
  cvFreenectDevice& device = freenect.createDevice<cvFreenectDevice>(0);

  device.startVideo();
  device.startDepth();

  VideoWriter outStream("./out.mjpg", CV_FOURCC('M','J','P','G'), 30, depthMat.size());

  while (true) {
    device.getDepth(depthMat);
    device.getDepth(rgbMat);
    depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);
    outStream.write(depthf);
  }

  device.stopVideo();
  device.stopDepth();
  return 0;
}
