#include <opencv2/core/core.hpp>
#include <libfreenect.hpp>
#include "mutex.hpp"

class cvFreenectDevice : public Freenect::FreenectDevice {
  public:
    cvFreenectDevice(freenect_context *_ctx, int _index) {
      for( unsigned int i = 0 ; i < 2048 ; i++) {
        float v = i/2048.0;
        v = std::pow(v, 3)* 6;
        m_gamma[i] = v*6*256;
      }
    }

    bool getVideo(Mat& output) {
      m_rgb_mutex.lock();
      if(m_new_rgb_frame) {
        cv::cvtColor(rgbMat, output, CV_RGB2BGR);
        m_new_rgb_frame = false;
        m_rgb_mutex.unlock();
        return true;
      } else {
        m_rgb_mutex.unlock();
        return false;
      }
    }

    bool getDepth(Mat& output) {
        m_depth_mutex.lock();
        if(m_new_depth_frame) {
          depthMat.copyTo(output);
          m_new_depth_frame = false;
          m_depth_mutex.unlock();
          return true;
        } else {
          m_depth_mutex.unlock();
          return false;
        }
      }

    //Callbacks for FreenectDevice below
    void VideoCallback(void* _rgb, uint32_t timestamp) {
      m_rgb_mutex.lock();
      uint8_t* rgb = static_cast<uint8_t*>(_rgb);
      rgbMat.data = rgb;
      m_new_rgb_frame = true;
      m_rgb_mutex.unlock();
    };

    void DepthCallback(void* _depth, uint32_t timestamp) {
      m_depth_mutex.lock();
      uint16_t* depth = static_cast<uint16_t*>(_depth);
      depthMat.data = (uchar*) depth;
      m_new_depth_frame = true;
      m_depth_mutex.unlock();
    }

  private:
    std::vector<uint8_t> m_buffer_depth;
    std::vector<uint8_t> m_buffer_rgb;
    std::vector<uint16_t> m_gamma;
    Mat depthMat;
    Mat rgbMat;
    Mat ownMat;
    Mutex m_rgb_mutex;
    Mutex m_depth_mutex;
    bool m_new_rgb_frame;
    bool m_new_depth_frame;
};
