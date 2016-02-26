#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <mutex>
#include "libfreenect.hpp"
#include "cvFreenect.hpp"

using namespace cv;
using namespace std;

cvFreenectDevice::cvFreenectDevice(freenect_context *_ctx, int _index)
  : Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT),
  m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false),
  m_new_depth_frame(false), depthMat(Size(640,480),CV_16UC1),
  rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
  ownMat(Size(640,480),CV_8UC3,Scalar(0)) {
  for( unsigned int i = 0 ; i < 2048 ; i++) {
    float v = i/2048.0;
    v = pow(v, 3)* 6;
    m_gamma[i] = v*6*256;
  }
}

bool cvFreenectDevice::getVideo(Mat& output) {
  lock_guard<mutex> lock(m_rgb_mutex);
  if(m_new_rgb_frame) {
    cv::cvtColor(rgbMat, output, CV_RGB2BGR);
    m_new_rgb_frame = false;
    return true;
  } else {
    return false;
  }
}

bool cvFreenectDevice::getDepth(Mat& output) {
  lock_guard<mutex> lock(m_depth_mutex);
    if(m_new_depth_frame) {
      depthMat.copyTo(output);
      m_new_depth_frame = false;
      return true;
    } else {
      return false;
    }
  }

//Callbacks for FreenectDevice below
void cvFreenectDevice::VideoCallback(void* _rgb, uint32_t timestamp) {
  lock_guard<mutex> lock(m_rgb_mutex);
  uint8_t* rgb = static_cast<uint8_t*>(_rgb);
  rgbMat.data = rgb;
  m_new_rgb_frame = true;
};

void cvFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp) {
  lock_guard<mutex> lock(m_depth_mutex);
  uint16_t* depth = static_cast<uint16_t*>(_depth);
  depthMat.data = (uchar*) depth;
  m_new_depth_frame = true;
}
