/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "engine/window.hpp"

#include <iostream>
#include <cassert>

#include "application/application.hpp"

Window::Window(WindowProperties properties, Application* owner){
	glfwSetErrorCallback(Window::ErrorCallback);

	glfwInit();

	SetWindowHints();

	GLFWwindow* window = glfwCreateWindow(properties.width, properties.height, properties.title,
		nullptr, nullptr);
	glfwWindow.reset(window);

	SetInputCallbacks();
	SetOwner(owner);

	glfwMakeContextCurrent(glfwWindow.get());

	int gladLoadResult = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	assert(gladLoadResult == 1);

	glfwSwapInterval(1);
}

Window::~Window(){
}

void Window::Hide(){
	glfwHideWindow(glfwWindow.get());
}

void Window::Show(){
	glfwShowWindow(glfwWindow.get());
	glfwFocusWindow(glfwWindow.get());
}

WindowDimensions Window::GetSize(){
	WindowDimensions size;
	glfwGetFramebufferSize(glfwWindow.get(), &size.width, &size.height);
	return size;
}

bool Window::IsOpen(){
	return !glfwWindowShouldClose(glfwWindow.get());
}

void Window::FlagWindowToClose(){
	glfwSetWindowShouldClose(glfwWindow.get(), GLFW_TRUE);
}

void Window::SetSwapInterval(int swapInterval){
	glfwSwapInterval(swapInterval);
}

void Window::SetInputMode(int mode, int value){
	glfwSetInputMode(glfwWindow.get(), mode, value);
}

void Window::SetTitle(const std::string & title){
	glfwSetWindowTitle(glfwWindow.get(), title.c_str());
}

void Window::SwapBuffers(){
	glfwSwapBuffers(glfwWindow.get());
}

void Window::SetOwner(Application * owner){
	glfwSetWindowUserPointer(glfwWindow.get(), static_cast<void*>(owner));
}

void Window::SetWindowHints(){
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	const int glMajorVersion = 4, glMinorVersion = 5;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinorVersion);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Can't get resizing of framebuffer textures to work so resize disabled
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE); // Enabled to ensure SRGB texture loading works properly (not using SRGB Framebuffer)
}

void Window::SetInputCallbacks(){
	glfwSetKeyCallback(glfwWindow.get(), Window::KeyboardCallback);
	glfwSetCursorPosCallback(glfwWindow.get(), Window::MouseMovementCallback);
	glfwSetFramebufferSizeCallback(glfwWindow.get(), Window::FramebufferSizeCallback);
}

void Window::ErrorCallback(int code, const char * description){
	std::cout << "GLFW error: " << description << "\nCode: " << code << std::endl;
}

void Window::KeyboardCallback(GLFWwindow * window, int key, int scancode, int action, int mods){
	auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	application->SetKeyboardState(key, scancode, action, mods);
}

void Window::MouseMovementCallback(GLFWwindow * window, double mouseX, double mouseY){
	auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	application->DoMouseInput(mouseX, mouseY);
}

void Window::FramebufferSizeCallback(GLFWwindow * window, int width, int height){
	auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	application->DoFramebufferResize(width, height);
}
