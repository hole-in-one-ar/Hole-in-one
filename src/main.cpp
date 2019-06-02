#include "Render/Render.h"
#include "Render/Window.h"

#include "Physics/PhysicsBall.h"
#include "Physics/Hole.h"
#include "Physics/Physics.h"
#include "Physics/Vec3.h"

#include "Vision/Tracking.h"

int main() {
	Window window(1280, 720, "app");
	Render render(1280, 720);
	Physics physics;
	Tracking tracking;
	//render.setCamera(Transform::lookAt(Vector3{ 0,3,1 }, Vector3{ 0,0,0 }));
	//render.setCamera(Transform::lookAt(Vector3{ 20,0,6 }, Vector3{ 0,0,4 }));
	tracking.resetCamera([&](Transform m) -> bool {
		//render.setCamera(Transform::lookAt(Vector3{ 20,0,6 }, Vector3{ 0,0,4 }));
		render.setCamera(m);
		return true;
	});

	HolePos hole{ {0,0}, 0.02f };
	physics.setHole(hole.p.x, hole.p.y, hole.r);
	//physics.addBalls(Vec3(0, -5, 12), 2, Vec3(0, 200, 0), 1);
	//physics.addBalls(Vec3(0, 5, 12), 2, Vec3(0, -200, 0), 500);
	float t = 0;
	while (window.alive()) {
		tracking.update();
		t += 0.01;
		render.drawBackground(tracking.cameraImage);
		render.drawHole(hole);
		for (const auto &b : physics.getBalls()) {
			auto p = b.getOrigin();
			float r = b.getRadium();
			render.drawBall({ {p.x,p.y,p.z}, r });
		}

		Vector3 p;
		p = tracking.controller().getOrigin();
		std::cout << p.x << "," << p.y << "," << p.z << std::endl;
		render.drawBall({ p, 0.02f });

		for(int i=0;i<20;i++) physics.simulation();
		window.refresh();
	}
	return 0;
}

