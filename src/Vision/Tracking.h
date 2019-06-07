#pragma once

#include "Vision.h"
#include "../Render/Util.h"

class Tracking {
	Vision vision;
	std::function<bool(Transform)> cb;
	Transform cameraMatrixInv;
	Transform relativeController;
	int aliveCount;
public:
	Transform calibratorM, controllerM;
	Tracking();
	cv::Mat& cameraImage;
	bool aliveController();
	void update();
	void resetCamera(std::function<bool(Transform)> callback);
	Transform controller() const;
};
