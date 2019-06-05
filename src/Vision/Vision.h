#include <iostream>
#include <vector>
#include <math.h>

#include <opencv2/opencv.hpp> /// include most of core headers
#include <opencv2/core.hpp> /// include most of core headers
#include <opencv2/imgproc.hpp> /// include GUI-related headers
#include <opencv2/highgui.hpp> /// include GUI-related headers

using Contour = std::vector<cv::Point>;
using ContourList = std::vector<Contour>;
using ContourHierarchy = std::vector<cv::Vec4i>;
using Points = std::vector<cv::Point2f>;
using Line = cv::Vec4f;
using Lines = std::vector<cv::Vec4f>;
using Pose = float[16];

class Marker {
public:
  int code; //0:calibration, 1:controller
  Pose resultMatrix;
  void print_matrix() {
		const auto &mat = resultMatrix;
		const int kSize = 4;
		for (int r = 0; r < kSize; ++r) {
			for (int c = 0; c < kSize; ++c) {
				std::cout << mat[c + kSize * r] << " ";
			}
			std::cout << std::endl;
		}
	}
};

using MarkerList = std::vector<Marker>;

class Vision {
private:
  bool is_calibration = true;
  const int marker_size = 6;
  cv::VideoCapture cap;
  void initVideoStream(cv::VideoCapture &cap);
  int subpixSampleSafe(const cv::Mat img, cv::Point2f point);
  cv::Point2f findPeakByFitting(cv::Mat img);
  bool isValidRect(Contour c);
  bool isValidMarker(cv::Mat img);
  int getRotation(cv::Mat img);
  cv::Mat rotateMarker(cv::Mat img, int rot);
public:
  Vision();
  cv::Mat img_camera;
  cv::Mat img_marked;
  cv::Mat img_marker;
  MarkerList marker_list;
  int updateCamera();
  void detectMarker();
  int end();
};