#include "Vision.h"
#include "PoseEstimation.h"

// Debug flags
bool is_contour_drawn = false;
bool is_corner_drawn = false;
bool is_edge_point_drawn = false;
bool is_stripe_drawn = false;
bool is_peak_drawn = false;
bool is_corrected_edge_drawn = false;
bool is_corrected_corner_drawn = false;
bool is_marker_drawn = true;


// Drawing Settings
const bool kIsLineClosed = true;
const cv::Scalar kEdgeColor(0, 50, 200);
const	int kThickness = 4;
const cv::Scalar kCircleColor(0, 255, 0);
const int kCircleSize = 5;
cv::Scalar kEdgePointColor(0, 0, 255);

const int kNumOfEdgePoints = 6; //the number of squares which compose one edge
int kMarkerLength = 100;
const std::string gameWinName = "Game Window";
const std::string debugWinName = "Debug Window";



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
cv::Point2f Vision::findPeakByFitting(cv::Mat img) {
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

  const int image_size = img_marked.rows*img_marked.cols;
  const int marker_size_min = (int)(image_size*0.001);
  const int marker_size_max = (int)(image_size*0.99);
  const int num_of_corners = 4;
  const bool is_valid = 
    (c.size() == num_of_corners) &&
    (marker_area < marker_size_max) &&
    (marker_area > marker_size_min) &&
    (cv::isContourConvex(c));
  return is_valid;
}

bool Vision::isValidMarker(cv::Mat img) {
  //6 * 6 marker
  //most outer pixel value is always black.
  //In (1,1),(1,4),(4,1),(4,4), one is black, the others are white.
  bool is_valid = true;
  for (int j = 0; j < 6; j++) {
    is_valid &= (img.at<uchar>(0, j) == 0);
    is_valid &= (img.at<uchar>(5, j) == 0);
    is_valid &= (img.at<uchar>(j, 0) == 0);
    is_valid &= (img.at<uchar>(j, 5) == 0);
  }
  int check = 0;
  check += (int)img.at<uchar>(1, 1);
  check += (int)img.at<uchar>(1, 4);
  check += (int)img.at<uchar>(4, 1);
  check += (int)img.at<uchar>(4, 4);
  if (check != 255*3) {
    is_valid = false;
  }
  return is_valid;
}

int getMarkerCode(cv::Mat marker) {
  //loose implement
  int p1 = (int)marker.at<uchar>(2, 2);
  int p2 = (int)marker.at<uchar>(2, 3);
  int p3 = (int)marker.at<uchar>(3, 2);
  int p4 = (int)marker.at<uchar>(3, 3);
  if (p1+p2+p3+p4 == 255*2) {
    return 1; //Controller
  } else if (p1+p2+p3+p4 == 255) {
    return 0; //Calibration
  } else {
    return -1;
  }
}

void Vision::detectMarker() {
  cv::Mat img_gray;
  cv::Mat img_filtered;

  ContourList contours;
  ContourHierarchy hierarchy;
  ContourList rects;

  marker_list.clear();
  img_marked = img_camera.clone();

  cv::cvtColor(img_marked, img_gray, cv::COLOR_BGR2GRAY );
  cv::adaptiveThreshold(img_gray, img_filtered, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 33, 5);
  cv::findContours(img_filtered, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

  for (auto contour: contours) {
    Contour approx; //approximated contour
    Lines preciseEdge; //fitted lines
    Points preciseCorners; //fitted corner

    //Approximate contour
    float epsilon = 0.05 * cv::arcLength(contour, true);
    cv::approxPolyDP(contour, approx, epsilon, true);
    //filter contour
    if (!isValidRect(approx)) {
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
    cv::Mat marker_show = cv::Mat(160, 160, CV_8UC1);

    cv::Point2f transformed1(0, 0);
    cv::Point2f transformed2(marker_gray.cols, 0);
    cv::Point2f transformed3(marker_gray.cols, marker_gray.rows);
    cv::Point2f transformed4(0, marker_gray.rows);
    transformedCorners.push_back(transformed1);
    transformedCorners.push_back(transformed2);
    transformedCorners.push_back(transformed3);
    transformedCorners.push_back(transformed4);

    cv::Mat perspectiveTransform = cv::getPerspectiveTransform(preciseCorners, transformedCorners);
    cv::Size markerSize(marker_gray.cols, marker_gray.rows);
    warpPerspective(img_gray, marker_gray, perspectiveTransform, markerSize);
    cv::Scalar mean = cv::mean(marker_gray);
    cv::threshold(marker_gray, marker_filtered, mean[0], 255, cv::THRESH_BINARY);
    cv::resize(marker_filtered, img_marker, cv::Size(6, 6), 0, 0, cv::INTER_LINEAR);
    if(isValidMarker(img_marker)){
      cv::resize(img_marker, marker_show, cv::Size(160, 160), 0, 0, cv::INTER_NEAREST);
      cv::imshow(debugWinName, marker_show);
      if (is_marker_drawn) {
        for (int i = 0; i < 4; i++) {
          cv::line(img_marked, preciseCorners[i], preciseCorners[(i+1)%4], kEdgePointColor, 2);
        }
      }
      Marker marker;
      marker.code = getMarkerCode(img_marker);
      estimateSquarePose(marker.resultMatrix, &preciseCorners.front(), kMarkerLength);
      marker.print_matrix();
      marker_list.push_back(marker);
    }
  } //loop of each contour
}

void Vision::execCalibration() {
  //finding calibration marker 
  //set a camera position
  
}

void Vision::execGame() {

}

int Vision::init() {
  std::cout << "Startup (Press ESC to quit)" << std::endl;
	cv::namedWindow(gameWinName, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(debugWinName, cv::WINDOW_AUTOSIZE);
  initVideoStream(cap);
  return 0;
}

int Vision::updateCamera() {
  cap >> img_camera;
  if(img_camera.empty()){
    printf("Could not query frame. Trying to reinitialize.\n");
    initVideoStream(cap);
    cv::waitKey(1000); /// Wait for one sec.
    return -1;
  }
  return 0;
}

int Vision::end() {
  cv::destroyAllWindows();
  return 0;
}