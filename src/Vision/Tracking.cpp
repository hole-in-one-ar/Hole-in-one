#include "Tracking.h"
#include <chrono>

Tracking::Tracking() : cameraImage(vision.img_camera), aliveCount(0) {}

void Tracking::update() {
	vision.updateCamera();
	auto s = std::chrono::high_resolution_clock::now();
	vision.detectMarker();
	auto e = std::chrono::high_resolution_clock::now();
	auto du = e - s;
	std::cout << du / std::chrono::milliseconds(1) << "ms" << std::endl;
	
	if(aliveCount > 0) aliveCount--;
	for (auto marker : vision.marker_list) {
		if (marker.code == 0) {
			// calibrator
			for (int i = 0; i < 16; i++) {
				calibratorM.m[i] = marker.resultMatrix[i];
			}
			bool resetCamera = cb(calibratorM);
			if (resetCamera) {
				cameraMatrixInv = calibratorM.inverse();
			}
		} else {
			// controller
			if (aliveController()) {
				for (int i = 0; i < 16; i++) {
					controllerM.m[i] = marker.resultMatrix[i] * 0.4 + controllerM.m[i] * 0.6;
				}
			} else {
				for (int i = 0; i < 16; i++) {
					controllerM.m[i] = marker.resultMatrix[i];
				}
			}
			relativeController = controllerM.transfer(cameraMatrixInv);
			aliveCount = 10;
		}
	}
}

void Tracking::resetCamera(std::function<bool(Transform)> callback) {
	cb = callback;
}

bool Tracking::aliveController() {
	return aliveCount > 0;
}

Transform Tracking::controller() const {
	return relativeController;
}