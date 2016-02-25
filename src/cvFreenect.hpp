#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <libfreenect>
#include <mutex>

class cvFreenectDevice : public Freenect::FreenectDevice {
  public:
    cvFreenectDevice(freenect_context *_ctx, int _index)

    bool getVideo(Mat& output);
    bool getDepth(Mat& output);

    //Callbacks for FreenectDevice below
    void VideoCallback(void* _rgb, uint32_t timestamp)
    void DepthCallback(void* _depth, uint32_t timestamp)

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
