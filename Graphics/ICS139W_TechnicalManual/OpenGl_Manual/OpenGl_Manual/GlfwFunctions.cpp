#include "GlfwFunctions.h"

#include "Utility.h"

#include <iostream>

GLFWwindow * glfw::CreateWindow( unsigned width, unsigned height, const char * title )
{
    // This hint must be included to let GLFW know that the hints are being set for an OPENGL context 
    // and not the OPENGL ES API (or NO_API).
    glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API ); 

    // Request a context with version 4.5, with deprecated functionality removed.
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE );
    return glfwCreateWindow( width, height, title, nullptr, nullptr );
}

void glfw::HandleError( int errorCode, const char * errorDescription )
{
    std::cerr << "An error occurred in GLFW (" << errorCode << ": " << errorDescription << ")" << std::endl;
}

void glfw::HandleKeypress( GLFWwindow * window, int keyCode, int scanCode, int action, int modKeys )
{
    switch ( keyCode )
    {
        case GLFW_KEY_ESCAPE:
        {
            if ( action == GLFW_PRESS )
            {
                // The window will close when the user presses escape
                glfwSetWindowShouldClose( window, GLFW_TRUE );
            }
        }
    }
}

void glfw::HandleWindowResize( GLFWwindow * window, int newWidth, int newHeight )
{
    // Resize OpenGL's viewport rectangle so the rasterized output image fits within the window
    glViewport( 0, 0, newWidth, newHeight );

    util::Camera* camera = static_cast< util::Camera* >( glfwGetWindowUserPointer( window ) );
    camera->projectionMatrix = glm::perspective( glm::radians( 45.0f ), static_cast< float >( newWidth ) / newHeight, 0.1f, 100.0f );

}
