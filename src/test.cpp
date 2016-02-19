#include <iostream>

#include <opencv2/core/core.hpp>

#include "cvFreenect.cpp"

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

  while (true) {
    device.getVideo(rgbMat);
    device.getDepth(depthMat);
  }
  
  device.stopVideo();
  device.stopDepth();
  return 0;
}

