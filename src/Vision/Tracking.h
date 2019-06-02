#pragma once

#include "Vision.h"
#include "../Render/Util.h"

class Tracking {
	Vision vision;
	std::function<bool(Transform)> cb;
	Transform cameraMatrixInv;
	Transform relativeController;
public:
	Transform calibratorM, controllerM;
	Tracking();
	cv::Mat& cameraImage;
	void update();
	void resetCamera(std::function<bool(Transform)> callback);
	Transform controller() const;
};
