/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/
#pragma once

#include <string>
#include <map>
#include <memory>

#include "engine/timer.hpp"
#include "engine/window.hpp"
#include "utility/model_types.hpp"

class WorldObject;

// Serves as entry point to a demo of the project rendering framework.
class Application
{
public:
    Application();
    ~Application();

	// Sets the state (press/release/repeat) of the key in the key map 
    void SetKeyboardState( int key, int scancode, int action, int mods );
	// Uses mouse coordinates to control camera movement
    void DoMouseInput( double mouseX, double mouseY );
	// Handles window resizes, ensuring the viewport and projection matrix are updated correctly
    void DoFramebufferResize( int width, int height );

	// Runs the demo
    void Run();

	// Returns true if demo is running
    bool IsRunning();
private:
	void RunDemo();

	// Initialization functions to set up OpenGL objects needed to render scene
	void InitShaders();
	void InitModels();
	void InitLights();
	void InitWorldObjects();
	void InitGeometryBuffer();
	void InitLightPassFramebuffer();
	void InitSkybox();
	void InitBrdfLut();


	// Does the actual keyboard input
    void DoKeyboardInput();

	// Stores keypress state of keyboard
    std::map<int, unsigned short> keys;

    std::unique_ptr<Window> window;
    std::unique_ptr<Timer> timer;

	std::vector<std::unique_ptr<WorldObject>> deferredRenderObjects;
	std::vector<std::unique_ptr<WorldObject>> forwardRenderObjects;

	bool cursorVisible;
    bool isRunning;
};

