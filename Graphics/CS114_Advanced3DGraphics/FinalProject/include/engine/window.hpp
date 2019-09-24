/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include "engine/timer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>

class Application;

struct WindowProperties
{
	int width;
	int height;
	const char* title;
};

struct WindowDimensions
{
	int width;
	int height;
};

// Abstracts GLFW's GLFWWindow and related functionality
class Window
{
public:
	Window(WindowProperties properties, Application* owner);
	~Window();

	void Hide();
	void Show();

	WindowDimensions GetSize();

	bool IsOpen();

	void FlagWindowToClose();

	void SetSwapInterval(int swapInterval);
	void SetInputMode(int mode, int value);
	void SetTitle(const std::string& title);

	void SwapBuffers();
private:
	void SetOwner(Application* owner);

	void SetWindowHints();

	static void ErrorCallback(int code, const char* description);

	void SetInputCallbacks();
	static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseMovementCallback(GLFWwindow* window, double mouseX, double mouseY);
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

	struct GlfwWindowDeleter
	{
		void operator()(GLFWwindow* window){
			glfwDestroyWindow(window);
		}
	};
	std::unique_ptr<GLFWwindow, GlfwWindowDeleter> glfwWindow;
};