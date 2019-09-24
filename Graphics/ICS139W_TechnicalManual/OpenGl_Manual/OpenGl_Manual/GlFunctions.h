#pragma once

#include <glad/glad.h>

namespace gl
{
    struct Vec3f
    {
        float x, y, z;
    };
    struct Vertex
    {
        Vec3f position;
        Vec3f color;
    };

    struct VertexBufferFormat
    {
        GLuint bindingIndex;
        GLintptr offset;
        GLuint stride;
    };

    struct AttributeFormat
    {
        GLuint size;
        GLenum type;
        GLboolean normalized;
        GLuint relativeOffset;
    };

    struct VertexArrayAttribute
    {
        AttributeFormat format;
        GLuint vertexBindingIndex;
        GLuint attributeIndex;
    };

    const GLubyte* GetGraphicsDeviceName();
    const GLubyte* GetOpenGlVersion();
    
    GLuint CreateVertexBuffer( const Vertex* vertices, unsigned numVertices );

    GLuint CreateElementBuffer( const unsigned* indices, unsigned numIndices );

    GLuint CreateVertexArrayObject();

    void BindVertexBufferToVertexArray( GLuint vertexArray, GLuint vertexBuffer, VertexBufferFormat bufferFormat );

    void BindElementBufferToVertexArray( GLuint vertexArray, GLuint elementBuffer );

    void SetVertexArrayAttribute( GLuint vertexArray, VertexArrayAttribute attribute );   

    GLuint CreateShader( GLenum type, const GLchar** source );
    GLuint CreateBasicShaderProgram( GLuint vertShader, GLuint fragShader );

    void SetProgramUniformMat4( GLuint shaderProgram, const char* name, const GLfloat* value );
}
