#include "Render/Render.h"
#include "Render/Window.h"

#include "Physics/PhysicsBall.h"
#include "Physics/Hole.h"
#include "Physics/Physics.h"
#include "Physics/Vec3.h"

int main() {
	Window window(800, 600, "app");
	Render render(800, 600);
	Physics physics;

	physics.setHole(0, 0, 3);
	physics.addBalls(Vec3(0, -5, 12), 2, Vec3(0, 200, 0), 1);
	physics.addBalls(Vec3(0, 5, 12), 2, Vec3(0, -200, 0), 500);
	while (window.alive()) {
		render.setCamera(Transform::lookAt(Vector3{ 10,0,10 }, Vector3{ 0,0,0 }));
		render.drawHole({ Vector2{0,0}, 3.0f });
		for (const auto &b : physics.getBalls()) {
			auto p = b.getOrigin();
			std::cout << p.z << std::endl;
			render.drawBall({ {p.x,p.y,p.z}, 1.f });
		}
		for(int i=0;i<20;i++) physics.simulation();
		window.refresh();
	}
	return 0;
}

