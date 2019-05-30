#pragma once

#include <GLFW/glfw3.h>

using uint = unsigned int;

class Window {
	GLFWwindow* window;
public:
	Window(uint w, uint h, const char* title);
	~Window();
	bool alive();
	void refresh();
	uint width();
	uint height();
};