#include "Render/Render.h"
#include "Render/Window.h"

#include "Physics/PhysicsBall.h"
#include "Physics/Hole.h"
#include "Physics/Physics.h"
#include "Physics/Vec3.h"

int main() {
	Window window(960, 540, "app");
	Render render(960, 540);
	Physics physics;

	HolePos hole{ {0,0}, 3 };
	physics.setHole(hole.p.x, hole.p.y, hole.r);
	physics.addBalls(Vec3(0, -5, 12), 2, Vec3(0, 200, 0), 1);
	physics.addBalls(Vec3(0, 5, 12), 2, Vec3(0, -200, 0), 500);
	float t = 0;
	while (window.alive()) {
		t += 0.01;
		render.setCamera(Vector3{ 20,0,6 }, Transform::lookAt(Vector3{ 20,0,6 }, Vector3{ 0,0,4 }));
		render.drawBackground({});
		render.drawHole(hole);
		for (const auto &b : physics.getBalls()) {
			auto p = b.getOrigin();
			float r = b.getRadium();
			render.drawBall({ {p.x,p.y,p.z}, r });
		}
		for(int i=0;i<20;i++) physics.simulation();
		window.refresh();
	}
	return 0;
}

