#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "libfreenect.hpp"
#include <mutex>

using namespace cv;
using namespace std;

class cvFreenectDevice : public Freenect::FreenectDevice {
  public:
    cvFreenectDevice(freenect_context *_ctx, int _index);

    bool getVideo(Mat& output);
    bool getDepth(Mat& output);

    //Callbacks for FreenectDevice below
    void VideoCallback(void* _video, uint32_t timestamp);
    void DepthCallback(void* _depth, uint32_t timestamp);

  private:
    vector<uint8_t> m_buffer_depth;
    vector<uint8_t> m_buffer_video;
    vector<uint16_t> m_gamma;
    bool m_new_video_frame;
    bool m_new_depth_frame;
    Mat depthMat;
    Mat videoMat;
    Mat ownMat;
    std::mutex m_video_mutex;
    std::mutex m_depth_mutex;
};
