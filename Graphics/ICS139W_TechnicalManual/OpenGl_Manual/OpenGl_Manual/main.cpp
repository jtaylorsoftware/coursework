// Opengl 4.5 Tutorial For 139W Technical Manual

#include "GlfwFunctions.h"
#include "GlFunctions.h"
#include "Utility.h"

#include <iostream>
#include <string>

int main()
{
    glfwSetErrorCallback( glfw::HandleError );

    if ( glfwInit() != GLFW_TRUE )
    {
        return 1;
    }

    const unsigned windowWidth = 640, windowHeight = 360; // 16:9 resolution used
    GLFWwindow* window = glfw::CreateWindow( windowWidth, windowHeight, "OpenGL 4.5 Core" );
    if ( window == nullptr )
    {
        glfwTerminate();
        return 1;
    }

    glfwSetKeyCallback( window, glfw::HandleKeypress );
    glfwSetWindowSizeCallback( window, glfw::HandleWindowResize );

    // Set the application's camera as the user pointer (so we can change it on window resize events)
    util::Camera camera;
    glfwSetWindowUserPointer( window, &camera );

    // Use glad library to load OpenGL functions - requires passing the process address from GLFW to the loader.
    // You could go without a loader like glad, but then on some systems (Windows) you could only use OpenGL 1.2.
    glfwMakeContextCurrent( window );
    gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress );

    glfwSwapInterval( 1 ); // Enables vertical sync (the default framebuffer will be swapped once per screen refresh)

    // Display the graphics device and version of the created OpenGL context for logging purposes
    // and to ensure OpenGL 4.5+ is supported.
    std::cout << "Graphics Device: " << gl::GetGraphicsDeviceName() << "\nOpenGL version: " << gl::GetOpenGlVersion() << std::endl;

    // Define the data used for the cube and set up the Vertex Buffer Object (VBO), Vertex Array Object (VAO) and Element/Indices buffer
    // for the cube.
    static gl::Vertex cubeVertices[] =
    {
        // Note - we can draw the sides, top, and bottom by suppling vertices 
        // of a triangle out of the edges of the front and back faces.

        // front face positions and colors
        -1.0f,  -1.0f,  1.0f,       0.8f, 0.f, 0.f, // front is red
        1.0f,   -1.0f,  1.0f,       0.8f, 0.f, 0.f,
        1.0f,   1.0f,   1.0f,       0.8f, 0.f, 0.f,
        -1.0f,  1.0f,   1.0f,       0.8f, 0.f, 0.f,
        // back face positions and colors
        -1.0f,  -1.0f,  -1.0f,      0.f, 0.f, 0.8f, // back is blue
        1.0f,   -1.0f,  -1.0f,      0.f, 0.f, 0.8f,
        1.0f,   1.0f,   -1.0f,      0.f, 0.f, 0.8f,
        -1.0f,  1.0f,   -1.0f,      0.f, 0.f, 0.8f,
    };
    GLuint vertexBuffer = gl::CreateVertexBuffer( cubeVertices, ARRAYSIZE( cubeVertices ) );

    static GLuint cubeIndices[] =
    {
        // front
        0, 1, 2, 2, 3, 0,
        // back
        7, 6, 5, 5, 4, 7,
        // bottom
        4, 0, 3, 3, 7, 4,
        // top
        1, 5, 6, 6, 2, 1,
        // left
        4, 5, 1, 1, 0, 4,
        // right
        3, 2, 6, 6, 7, 3
    };
    GLuint elementBuffer = gl::CreateElementBuffer( cubeIndices, ARRAYSIZE( cubeIndices ) );

    GLuint vertexArrayObject = gl::CreateVertexArrayObject();

    gl::VertexBufferFormat bufferFormat;
    // This number must be used for matching the buffer data source (vertex buffer
    // and its binding point) to the VAO Attribute. The Attribute tells OpenGL how to 
    // interpret the data from the VBO at the given binding point. 
    bufferFormat.bindingIndex = 0;

    bufferFormat.offset = ( GLintptr ) nullptr; // The shader will start reading from the start of the source data
    bufferFormat.stride = sizeof( gl::Vertex ); // The shader will consider every sizeof(gl::Vertex) bytes to be another Vertex

    gl::BindVertexBufferToVertexArray( vertexArrayObject, vertexBuffer, bufferFormat );

    gl::VertexArrayAttribute attrib;
    attrib.attributeIndex = 0; // Use attribute index 0 of the VAO for position
    attrib.vertexBindingIndex = 0; // The source data is coming from VBO at binding point 0 (this is a constant in this example)

    attrib.format.normalized = GL_FALSE;
    attrib.format.type = GL_FLOAT;

    // size tells the vertex shader how many components each vector contains (1-4 components for x,y,z,w) in the source data.
    // If this number is more than the shader uses, the extra is ignored. If this number is less, then the extra components in the
    // shader are initialized from the default (0,0,0,1) vector. 
    attrib.format.size = 3;

    // relativeOffset tells the vertex shader where the data for the attribute begins in the current source vertex.
    // This is an offset per vertex rather than an offset into the source data as a whole.
    attrib.format.relativeOffset = offsetof( gl::Vertex, gl::Vertex::position );

    gl::SetVertexArrayAttribute( vertexArrayObject, attrib );

    attrib.format.relativeOffset = offsetof( gl::Vertex, gl::Vertex::color );
    attrib.attributeIndex = 1; // Use attribute index 1 of the VAO for color

    gl::SetVertexArrayAttribute( vertexArrayObject, attrib );

    gl::BindElementBufferToVertexArray( vertexArrayObject, elementBuffer );

    // Create shaders - for simplicty, we aren't going to read them from files
    const GLchar* vertexShaderSource =
        "#version 450 core\n"

        "layout( location = 0 ) in vec3 position;\n"
        "layout( location = 1 ) in vec3 color;\n"

        "layout( location = 0 ) out vec3 outColor;\n"

        // uniforms - must pass these in from the application
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"

        "void main()\n"
        "{\n"
        "    outColor = color;\n" // pass the color value through directly to fragment shader
        "    gl_Position = projection * view * model * vec4( position, 1.0 );\n" // screenspace position
        "}\n";

    const GLchar* fragShaderSource =
        "#version 450 core\n"

        "layout( location = 0 ) in vec3 color;\n"

        "out vec4 outColor;\n"

        "void main()\n"
        "{"
        "    outColor = vec4( color, 1.0 );\n" // directly use the vertex color as the fragment color
        "}\n";

    GLuint vertShader = gl::CreateShader( GL_VERTEX_SHADER, &vertexShaderSource );
    GLuint fragShader = gl::CreateShader( GL_FRAGMENT_SHADER, &fragShaderSource );
    GLuint shaderProgram = gl::CreateBasicShaderProgram( vertShader, fragShader );
    glDeleteShader( vertShader );
    glDeleteShader( fragShader );

    // Use the identity matrix as the model (to world) matrix
    glm::mat4 modelMatrix( 1.f );

    // Set up the camera
    camera.viewMatrix = glm::lookAt(
        glm::vec3( 0.f, 0.f, 5.f ),     // camera position
        glm::vec3( 0.f, 0.f, 0.f ),     // point the camera looks at
        glm::vec3( 0.f, 1.f, 0.f )      // up vector
    );

    camera.projectionMatrix = glm::perspective( glm::radians( 45.0f ), static_cast< float >( windowWidth ) / windowHeight, 0.1f, 100.0f );

    gl::SetProgramUniformMat4( shaderProgram, "model", glm::value_ptr( modelMatrix ) );
    gl::SetProgramUniformMat4( shaderProgram, "view", glm::value_ptr( camera.viewMatrix ) );
    gl::SetProgramUniformMat4( shaderProgram, "projection", glm::value_ptr( camera.projectionMatrix ) );

    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    glUseProgram( shaderProgram );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    while ( !glfwWindowShouldClose( window ) )
    {
        glfwPollEvents();
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Set the projection uniform every frame in case the window is resized
        gl::SetProgramUniformMat4( shaderProgram, "projection", glm::value_ptr( camera.projectionMatrix ) );

        // Rotate the cube around the Y-axis
        modelMatrix = glm::rotate( modelMatrix, glm::radians(1.0f), glm::vec3( 0.f, 1.f, 0.f ) );
        gl::SetProgramUniformMat4( shaderProgram, "model", glm::value_ptr( modelMatrix ) );

        glBindVertexArray( vertexArrayObject );
        glDrawElements( GL_TRIANGLES, ARRAYSIZE( cubeIndices ), GL_UNSIGNED_INT, nullptr );

        glfwSwapBuffers( window );
    }
    return 0;
}