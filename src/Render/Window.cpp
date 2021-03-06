#include "Window.h"
#include <stdexcept>

unsigned int glfwWindowWidth, glfwWindowHeight;

void glfwReshape(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glfwWindowWidth = width;
	glfwWindowHeight = height;
}

Window::Window(uint w, uint h, const char* title) {
	glfwWindowWidth = w;
	glfwWindowHeight = h;
	if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(w, h, title, NULL, NULL);
	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Failed to create window");
	}

	glfwSetFramebufferSizeCallback(window, glfwReshape);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 1);
	glClearStencil(0);
	glViewport(0, 0, w, h);
}

bool Window::alive() {
	return !glfwWindowShouldClose(window);
}

void Window::refresh() {
	glfwSwapBuffers(window);
	glfwPollEvents();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

Window::~Window() {
	glfwTerminate();
}

uint Window::width() {
	return glfwWindowWidth;
}
uint Window::height() {
	return glfwWindowHeight;
}