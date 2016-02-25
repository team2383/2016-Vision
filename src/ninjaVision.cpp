#include <iostream>
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <opencv2/core/core.hpp>

#include "cvFreenect.cpp"
#include "../streamer/mjpeg_server.hpp" //github.com/ethanrublee/streamer

using namespace cv;
using namespace std;

const size_t num_threads = 4;
const string doc_root = "./";

int main(int argc, char **argv) {
  Mat depthMat(Size(640,480),CV_16UC1);
  Mat depthf (Size(640,480),CV_8UC1);
  Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
  Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));

  Freenect::Freenect freenect;
  cvFreenectDevice& device = freenect.createDevice<cvFreenectDevice>(0);

  server_ptr s = init_streaming_server("0.0.0.0", "9090", doc_root, num_threads);
  streamer_ptr stmr(new streamer);
  register_streamer(s, stmr, "/stream_0");

  device.startVideo();
  device.startDepth();

  s->start();

  while (true) {
    bool hasMat = device.getDepth(depthMat);
    if (hasMat) {
      bool wait = false;
      int quality = 75;
      int n_viewers = stmr->post_image(depthMat, quality, wait);
      this_thread::sleep_for(chrono::milliseconds(20));
    }
  }

  device.stopVideo();
  device.stopDepth();
  return 0;
}

static void mjpeg_server () {


}
