#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <mutex>
#include <cmath>
#include <vector>
#include <iostream>
#include "libfreenect.hpp"
#include "cvFreenect.hpp"

using namespace cv;
using namespace std;

cvFreenectDevice::cvFreenectDevice(freenect_context *_ctx, int _index)
  : Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT),
  m_buffer_video(FREENECT_VIDEO_IR_8BIT), m_gamma(2048), m_new_video_frame(false),
  m_new_depth_frame(false), depthMat(Size(640,480),CV_16UC1),
  videoMat(Size(640,480), CV_8UC1, Scalar(0)),
  ownMat(Size(640,480),CV_8UC1,Scalar(0)) {
  for( unsigned int i = 0 ; i < 2048 ; i++) {
    float v = i/2048.0;
    v = pow(v, 3)* 6;
    m_gamma[i] = v*6*256;
  }
}

bool cvFreenectDevice::getVideo(Mat& output) {
  lock_guard<mutex> lock(m_video_mutex);
  if(m_new_video_frame) {
    cv::cvtColor(videoMat, output, CV_GRAY2RGB);
    m_new_video_frame = false;
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
void cvFreenectDevice::VideoCallback(void* _video, uint32_t timestamp) {
  lock_guard<mutex> lock(m_video_mutex);
  uint8_t* video = static_cast<uint8_t*>(_video);
  videoMat.data = video;
  m_new_video_frame = true;
};

void cvFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp) {
  lock_guard<mutex> lock(m_depth_mutex);
  uint16_t* depth = static_cast<uint16_t*>(_depth);
  depthMat.data = (uchar*) depth;
  m_new_depth_frame = true;
}
