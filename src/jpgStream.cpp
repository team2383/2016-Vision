#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace std;

vector<uchar> cvMat_to_mjpeg_buffer(cv::Mat frame, vector<uchar> jpegBuffer) {
  vector<int> params;
  
  params.push_back(CV_IMWRITE_JPEG_QUALITY);
  params.push_back(50);
  cv::imencode(".jpg", frame, jpegBuffer, params);
}
