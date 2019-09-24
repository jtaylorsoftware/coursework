#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#undef CreateWindow // Get rid of WinAPI macro definition

namespace glfw
{
    // Creates a GLFWWindow that has been set up for an OpenGL 4.5 Core context
    GLFWwindow* CreateWindow( unsigned width, unsigned height, const char* title );

    // Function that will be called when an internal error occurs in GLFW
    void HandleError( int errorCode, const char* errorDescription );

    // Function that will be called by GLFW when the program receives keyboard input
    void HandleKeypress( GLFWwindow* window, int keyCode, int scanCode, int action, int modKeys );

    // Function that will be called by GLFW when the window is resized
    void HandleWindowResize( GLFWwindow* window, int newWidth, int newHeight );
}
