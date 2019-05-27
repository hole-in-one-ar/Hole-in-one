#include "Render.h"
#include "Window.h"

int main() {
	Window window(800, 600, "app");
	Render render;
	while (window.alive()) {
		render.drawBall({ {0,0,0}, 1 });
		window.refresh();
	}
	return 0;
}

