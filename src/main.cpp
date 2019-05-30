#include "Render.h"
#include "Window.h"

int main() {
	Window window(800, 600, "app");
	Render render(800, 600);
	float t = 0;
	while (window.alive()) {
		t += 0.01;
		render.setCamera(Transform::lookAt(Vector3{ 4,0,1 }, Vector3{ 0,0,0 }));
		for (int i = 0; i < 12; i++) {
			float x = cos(i*3.1415926535 / 12 * 2);
			float y = sin(i*3.1415926535 / 12 * 2);
			float z = sin(t)*0.5+0.5;
			render.drawBall({ {x,y,z}, abs(x)*0.1f });
		}
		render.drawBall({ { 0,0,0 }, 0.1f });
		render.drawHole(Hole{ Vector2{0,0}, 1.0f });
		window.refresh();
	}
	return 0;
}

