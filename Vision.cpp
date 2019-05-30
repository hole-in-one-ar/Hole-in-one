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
using MarkerList = std::vector<cv::Mat>;

// Debug flags
bool is_contour_drawn = false;
bool is_corner_drawn = false;
bool is_edge_point_drawn = false;
bool is_stripe_drawn = false;
bool is_peak_drawn = false;
bool is_corrected_edge_drawn = false;
bool is_corrected_corner_drawn = false;


// Drawing Settings
const bool kIsLineClosed = true;
const cv::Scalar kEdgeColor(0, 50, 200);
const	int kThickness = 4;
const cv::Scalar kCircleColor(0, 255, 0);
const int kCircleSize = 5;
cv::Scalar kEdgePointColor(0, 0, 255);

const int kNumOfEdgePoints = 6; //the number of squares which compose one edge
const std::string gameWinName = "Game Window";
const std::string debugWinName = "Debug Window";

class Vision {
  private:
    bool is_calibration = true;
    const int marker_size = 6;
    MarkerList marker_list;
    void initVideoStream(cv::VideoCapture &cap);
    int subpixSampleSafe(const cv::Mat img, cv::Point2f point);
    cv::Point2f findPeakByFitting(cv::Mat img);
    void detectMarker();
    bool isValidRect(Contour c);
    void execCalibration();
    void execGame();
  public:
    cv::Mat img_camera;
    cv::Mat img_marked;
    cv::Mat img_marker;
    void start();
};

/*
 * Initialize video stream
 */
void Vision::initVideoStream( cv::VideoCapture &cap ) {
	if( cap.isOpened() )
		cap.release();

	cap.open(0); // open the default camera
	if ( cap.isOpened()==false ) {
		std::cout << "No webcam found, using a video file" << std::endl;
		cap.open("MarkerMovie.mpg");
		if ( cap.isOpened()==false ) {
			std::cout << "No video file found. Exiting."      << std::endl;
			exit(0);
		}
	}
}

/*
 * return subpix value
 */
int Vision::subpixSampleSafe(const cv::Mat img, cv::Point2f point) {
	if (point.x + 1 > img.cols || point.y + 1 > img.rows || point.x < 1 || point.y < 1) {
		return 0;
	}
	cv::Point2i points[4];
	points[0].x = (int)point.x;
	points[0].y = (int)point.y;
	points[1].x = (int)point.x + 1;
	points[1].y = (int)point.y;
	points[2].x = (int)point.x;
	points[2].y = (int)point.y + 1;
	points[3].x = (int)point.x + 1;
	points[3].y = (int)point.y + 1;
	double dx = point.x - std::floor(point.x);
	double dy = point.y - std::floor(point.y);
	double subpix = 0;
	double value1 = dx * dy * (double)(img.at<uchar>(points[0]));
	double value2 = (1 - dx) * dy * (double)(img.at<uchar>(points[1]));
	double value3 = dx * (1 - dy) * (double)(img.at<uchar>(points[2]));
	double value4 = (1 - dx) * (1 - dy) * (double)(img.at<uchar>(points[3]));
	subpix = value1 + value2 + value3 + value4;
	return (int)subpix;
}

/*
 * fit by least squares method and return the peak position
 */
cv::Point2f findPeakByFitting(cv::Mat img) {
	cv::Mat target = img.col(1); //col=1, row=n のベクトル
  //最小二乗法でフィッティングを行う。
	int n = target.rows;
	int sx4 = 0;
	int sx3 = 0;
	int sx2 = 0;
	int sx = 0;
	int sx2y = 0;
	int sxy = 0;
	int sy = 0;

	for (int i=0; i<n; i++) {
		int x = i;
		int y = (int)target.at<uchar>(i, 1);
		sx4  += x * x * x * x;
		sx3  += x * x * x;
		sx2  += x * x;
		sx   += x;
		sx2y += x * x * y;
		sxy  += x * y;
		sy   += y;
	}
	const cv::Matx33f left(sx4, sx3, sx2, sx3, sx2, sx, sx2, sx, n);
	const cv::Matx33f invmat = left.inv();
	const cv::Matx31f right(sx2y, sxy, sy);
	const cv::Matx31f ans = invmat * right;
	float a = ans(0, 0);
	float b = ans(1, 0);
	float c = ans(2, 0);
	float py = - b / (2 * a);
	cv::Point2f point(1, py);

	return point;
}

bool Vision::isValidRect(Contour c) {
  cv::Rect rect = cv::boundingRect(c);
	int marker_area = rect.area();

  const int image_size = img_camera.rows*img_camera.cols;
  const int marker_size_min = (int)(image_size*0.01);
  const int marker_size_max = (int)(image_size*0.5);
  const int num_of_corners = 4;
  const bool is_valid = 
    (c.size() == num_of_corners) &&
    (marker_area < marker_size_max) &&
    (marker_area > marker_size_min) &&
    (cv::isContourConvex(c));
  return is_valid;
}

void Vision::detectMarker() {
  cv::Mat img_gray;
  cv::Mat img_filtered;

  ContourList contours;
  ContourList rects;

  img_marked = img_camera.clone();

  cv::cvtColor(img_marked, img_gray, cv::COLOR_BGR2GRAY );
  cv::adaptiveThreshold(img_gray, img_filtered, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 33, 5);
  cv::findContours(img_filtered, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

  for (auto contour: contours) {
    Contour approx; //approximated contour
    Lines preciseEdge; //fitted lines
    Points preciseCorners; //fitted corner

    //Approximate contour
    auto epsilon = 0.05 * cv::arcLength(contour, true);
    cv::approxPolyDP(contour, approx, epsilon, true);

    //filter contour
    if (!isValidRect(contour)) {
      continue;
    }

    //Draw Contour
    if (is_contour_drawn) {
      cv::polylines(img_marked, approx, kIsLineClosed, kEdgeColor, kThickness);
    }
    
    //Correct the edge
    for (int i=0; i<4; i++) {
      Points line_points; //precise edge point
      Line line; //precise line
      cv::Point2f point1 = approx[i];
      cv::Point2f point2 = approx[(i+1)%4];

      //Draw Points
      if (is_corner_drawn) {
        cv::circle(img_marked, point1, kCircleSize, kCircleColor, cv::FILLED);
      }

      //Edge direction vector
      const double edge_x = (double)(point2.x - point1.x);
      const double edge_y = (double)(point2.y - point1.y);
      const double edge_len = (double)(std::sqrt(edge_x*edge_x + edge_y*edge_y));

      //One seventh of edge direction vector
      const double dx = (edge_x)/(kNumOfEdgePoints+1);
      const double dy = (edge_y)/(kNumOfEdgePoints+1);
      
      for (int j=0; j<kNumOfEdgePoints; j++) {
        cv::Point2f edge_point;
        cv::Size stripeSize;
        cv::Point2f stripeVecX; //parallel to the edge
        cv::Point2f stripeVecY; //vertical to the edge
        cv::Mat img_stripe;
        cv::Mat sobel_filtered;
        int half_w;
        int half_h;

        //Center of stripe
        edge_point.x = (double)(point1.x + dx*(j+1));
        edge_point.y = (double)(point1.y + dy*(j+1));

        //Draw edge point
        if (is_edge_point_drawn) {
          cv::circle(img_marked, edge_point, 2, kEdgePointColor, -1);
        }

        //define the size of stripe
        int stripeLength = (int)(0.8 * std::sqrt(dx*dx + dy*dy));
        if (stripeLength < 5) {
          stripeLength = 5;
        }
        stripeSize.width = 3;
        stripeSize.height = stripeLength;
        half_w = stripeSize.width / 2;
        half_h = stripeSize.height / 2;

        //Define stripe direction vector 
        double diffLength = std::sqrt(dx*dx + dy*dy);
        stripeVecX.x = dx / diffLength;
        stripeVecX.y = dy / diffLength;
        stripeVecY.x = stripeVecX.y;
        stripeVecY.y = -stripeVecX.x;

        //Get the origin of the label
        cv::Point2f stripeEdge = edge_point - stripeVecX*half_w - stripeVecY*half_h;
        cv::Point2f stripeOrigin = stripeEdge + stripeVecX * 0.5 + stripeVecY * 0.5;
        img_stripe = cv::Mat(stripeSize, CV_8UC1);

        //Pick Stripe
        for (int k=0; k<stripeSize.width; k++) {
          for (int l=0; l<stripeSize.height; l++) {
            cv::Point2f subPixel = stripeOrigin + k*stripeVecX + l*stripeVecY;
            if (is_stripe_drawn) {
              cv::circle(img_marked, subPixel, 1, cv::Scalar(255,255,0), -1);
            }
            int subpix = subpixSampleSafe(img_gray, subPixel);
            img_stripe.at<uchar>(l, k) = (uchar)subpix;
          }
        } //double loop of each subpix
        cv::Sobel(img_stripe, sobel_filtered, -1, 0, 1);

        cv::Point2f stripe_peak = findPeakByFitting(sobel_filtered);
        cv::Point2f camera_peak = stripeOrigin + stripeVecX + stripeVecY*stripe_peak.y;
        if (is_peak_drawn) {
          cv::circle(img_marked, camera_peak, 3, cv::Scalar(0, 0, 0), -1);
        }
        line_points.push_back(camera_peak);
      } //loop of each edge point

      
      cv::fitLine(line_points, line, cv::DIST_L12, 0, 0.01, 0.01);
      float vx = line[0];
      float vy = line[1];
      float x0 = line[2];
      float y0 = line[3];
      cv::Point2f center(x0, y0);
      cv::Point2f direction(vx, vy);
      if (is_corrected_edge_drawn) {
        cv::line(img_marked, center - direction*edge_len*2/3, center + direction*edge_len*2/3, kEdgePointColor, 2);
      }
      preciseEdge.push_back(line);
    } //loop of each edge

    for (int i=0; i<4; i++) {
      Line first = preciseEdge[i];
      Line second = preciseEdge[(i+1)%4];
      cv::Matx22f left(-first[1], first[0], -second[1], second[0]);
      cv::Matx21f right(-first[1]*first[2]+first[0]*first[3], -second[1]*second[2]+second[0]*second[3]);
      cv::Matx21f ans = (left.inv())*right;
      cv::Point2f preciseCorner(ans(0,0), ans(1,0));
      preciseCorners.push_back(preciseCorner);
      if (is_corrected_corner_drawn) {
        cv::circle(img_marked, preciseCorner, 5, cv::Scalar(255, 0, 0), cv::FILLED);
      }
    } //loop of each precise edge

    Points transformedCorners;
    cv::Mat marker_gray = cv::Mat(160, 160, CV_8UC1);
    cv::Mat marker_filtered = cv::Mat(160, 160, CV_8UC1);

    cv::Point2f transformed1(0, 0);
    cv::Point2f transformed2(marker_gray.cols, 0);
    cv::Point2f transformed3(marker_gray.cols, marker_gray.rows);
    cv::Point2f transformed4(0, marker_gray.rows);
    transformedCorners.push_back(transformed1);
    transformedCorners.push_back(transformed2);
    transformedCorners.push_back(transformed3);
    transformedCorners.push_back(transformed4);

    cv::Mat perspectiveTransform = cv::getPerspectiveTransform(preciseCorners, transformedCorners);
    cv::Size markerSize(marker_size, marker_size);
    warpPerspective(img_gray, marker_gray, perspectiveTransform, markerSize);
    cv::Scalar mean = cv::mean(marker_gray);
    cv::threshold(marker_gray, marker_filtered, mean[0], 255, cv::THRESH_BINARY);
    cv::resize(marker_filtered, img_marker, cv::Size(6, 6), 0, 0, cv::INTER_LINEAR);
    cv::resize(img_marker, img_marker, cv::Size(160, 160), 0, 0, cv::INTER_NEAREST);
    cv::imshow(debugWinName, img_marker);
    marker_list.push_back(img_marker.clone());
  } //loop of each contour
}

void Vision::execCalibration() {
  
}

void Vision::execGame() {

}
void Vision::start() {
  cv::VideoCapture cap;
  std::cout << "Startup (Press ESC to quit)" << std::endl;
	cv::namedWindow(gameWinName, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(debugWinName, cv::WINDOW_AUTOSIZE);
  initVideoStream(cap);

  while (1) {
    cap >> img_camera;
    if(img_camera.empty()){
			printf("Could not query frame. Trying to reinitialize.\n");
			initVideoStream(cap);
			cv::waitKey(1000); /// Wait for one sec.
			continue;
		}

    //main process
    detectMarker();
    if (is_calibration) {
      execCalibration();
    } else {
      execGame();
    }
    cv::imshow(gameWinName, img_camera);
    cv::imshow(debugWinName, img_marked);

    int key = cv::waitKey(10);
    if (key == 27) break;
  }

  cv::destroyAllWindows();
}

int main(){
  Vision v;
  v.start();
}