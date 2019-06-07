#include "Render/Render.h"
#include "Render/Window.h"

#include "Physics/PhysicsBall.h"
#include "Physics/Hole.h"
#include "Physics/Physics.h"
#include "Physics/Vec3.h"

#include "Vision/Tracking.h"

#include <random>

int main() {
	Window window(1280, 720, "app");
	Render render(1280, 720);
	Physics physics;
	Tracking tracking;
	//render.setCamera(Transform::lookAt(Vector3{ 0,3,1 }, Vector3{ 0,0,0 }));
	//render.setCamera(Transform::lookAt(Vector3{ 20,0,6 }, Vector3{ 0,0,4 }));
	bool holeIn = false;
	tracking.resetCamera([&](Transform m) -> bool {
		//render.setCamera(Transform::lookAt(Vector3{ 20,0,6 }, Vector3{ 0,0,4 }));
		if (holeIn) {
			return false;
		} else {
			render.setCamera(m);
			return true;
		}
	});

	HolePos hole{ {0,0}, 0.06f };
	physics.setHole(hole.p.x, hole.p.y, hole.r);
	//physics.addBalls(Vec3(0, -5, 12), 2, Vec3(0, 200, 0), 1);
	//physics.addBalls(Vec3(0, 5, 12), 2, Vec3(0, -200, 0), 500);
	float t = 0;
	std::random_device rd;
	std::mt19937 random(rd());
	std::uniform_real_distribution<float> uniform;
	while (window.alive()) {
		tracking.update();
		render.drawBackground(tracking.cameraImage);
		t += 0.1;
		if (tracking.aliveController()) {
			Transform tr = tracking.controller();
			Vector3 p = tr.getOrigin();
			Vector3 n = tr.getNormal();
			render.drawShooter(p, n, 0.02f);
			if (t > 1.0) {
				t = 0;
				float f = 100.0f + 50.0f * uniform(random);
				Vector3 v = { n.x*f, n.y*f, n.z*f };
				float r = 0.02f;
				if (p.z > r) {
					physics.addBalls(Vec3(p.x, p.y, p.z), r, Vec3(v.x, v.y, v.z), 1);
				}
			}
		}
		render.drawHole(hole);
		std::vector<int> removeIndices;
		unsigned int ix = 0;
		for (const auto &b : physics.getBalls()) {
			auto p = b.getOrigin();
			float r = b.getRadium();
			if (abs(p.x) < 1 && abs(p.y) < 1 && p.z > -1.0) {
				render.drawBall({ {p.x,p.y,p.z}, r }, hole);
			}
			if(abs(p.x) > 1 || abs(p.y) > 1 || p.z < 0.0) {
				removeIndices.push_back(ix);
				if (p.z < 0.0) {
					holeIn = true;
					std::cout << "Hole In!" << std::endl;
				}
			}
			ix++;
		}
		for (auto i : removeIndices) {
			physics.removeBall(i);
		}

		Vector3 p;
		p = tracking.controller().getOrigin();
		render.drawBall({ p, 0.005f }, hole);

		static float ti = 0;
		ti += 0.1f;
		//render.drawBall({ {0,0,(sin(ti)*0.5f+0.5f)*2.0f}, 1 });

		for(int i=0;i<10;i++) physics.simulation();
		window.refresh();
	}
	return 0;
}

