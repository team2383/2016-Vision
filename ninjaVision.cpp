#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "MyFreenect.cpp"
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
  MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

	namedWindow("rgb",CV_WINDOW_AUTOSIZE);
	namedWindow("depth",CV_WINDOW_AUTOSIZE);
	device.startVideo();
	device.startDepth();
	while (true) {
		device.getVideo(rgbMat);
		device.getDepth(depthMat);
		cv::imshow("rgb", rgbMat);
		depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);
		cv::imshow("depth",depthf);
	}
  device.stopVideo();
  device.stopDepth();
  return 0;
}
