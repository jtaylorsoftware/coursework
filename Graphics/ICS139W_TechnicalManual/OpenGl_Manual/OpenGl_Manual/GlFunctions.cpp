#include "GlFunctions.h"

#include <cassert>
#include <iostream>
#include <string>

const GLubyte * gl::GetGraphicsDeviceName()
{
    return glGetString( GL_RENDERER );
}

const GLubyte * gl::GetOpenGlVersion()
{
    return glGetString( GL_VERSION );
}

GLuint gl::CreateVertexBuffer( const Vertex * vertices, unsigned numVertices )
{
    GLuint buffer;
    glCreateBuffers( 1, &buffer );

    glNamedBufferData( buffer, numVertices * sizeof( Vertex ), vertices, GL_STATIC_DRAW );
    return buffer;
}

GLuint gl::CreateElementBuffer( const unsigned * indices, unsigned numIndices )
{
    GLuint buffer;
    glCreateBuffers( 1, &buffer );

    glNamedBufferData( buffer, numIndices * sizeof( unsigned ), indices, GL_STATIC_DRAW );
    return buffer;
}

GLuint gl::CreateVertexArrayObject()
{
    GLuint vertexArray;
    glCreateVertexArrays( 1, &vertexArray );
    return vertexArray;
}

void gl::BindVertexBufferToVertexArray( GLuint vertexArray, GLuint vertexBuffer, VertexBufferFormat bufferFormat )
{
    glVertexArrayVertexBuffer( vertexArray, bufferFormat.bindingIndex, vertexBuffer, bufferFormat.offset, bufferFormat.stride );
}

void gl::BindElementBufferToVertexArray( GLuint vertexArray, GLuint elementBuffer )
{
    glVertexArrayElementBuffer( vertexArray, elementBuffer );
}

void gl::SetVertexArrayAttribute( GLuint vertexArray, VertexArrayAttribute attribute )
{
    glVertexArrayAttribFormat( vertexArray, attribute.attributeIndex, attribute.format.size, attribute.format.type,
        attribute.format.normalized, attribute.format.relativeOffset );
    glVertexArrayAttribBinding( vertexArray, attribute.attributeIndex, attribute.vertexBindingIndex );
    glEnableVertexArrayAttrib( vertexArray, attribute.attributeIndex );
}

GLuint gl::CreateShader( GLenum type, const GLchar ** source )
{
    GLuint shader = glCreateShader( type );
    glShaderSource( shader, 1, source, nullptr );
    glCompileShader( shader );
    //GLint isCompiled;
    //glGetShaderiv( shader, GL_COMPILE_STATUS, &isCompiled );
    //if ( isCompiled != GL_TRUE )
    //{
    //    GLint logLength = 0;
    //    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );
    //    std::string log;
    //    log.resize( logLength );
    //    glGetShaderInfoLog( shader, logLength, &logLength, &log[ 0 ] );
    //    std::cerr << log << std::endl;
    //}
    return shader;
}

GLuint gl::CreateBasicShaderProgram( GLuint vertShader, GLuint fragShader )
{
    GLuint shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertShader );
    glAttachShader( shaderProgram, fragShader );
    glLinkProgram( shaderProgram );
    //GLint isValid;
    //glGetProgramiv( shaderProgram, GL_VALIDATE_STATUS, &isValid );
    //if ( isValid != GL_TRUE )
    //{
    //    GLint logLength = 0;
    //    glGetProgramiv( shaderProgram, GL_INFO_LOG_LENGTH, &logLength );
    //    std::string log;
    //    log.resize( logLength );
    //    glGetProgramInfoLog( shaderProgram, logLength, &logLength, &log[ 0 ] );
    //    std::cerr << log << std::endl;
    //}

    glDetachShader( shaderProgram, vertShader );
    glDetachShader( shaderProgram, fragShader );
    return shaderProgram;
}

void gl::SetProgramUniformMat4( GLuint shaderProgram, const char * name, const GLfloat* value )
{
    GLint location = glGetUniformLocation( shaderProgram, name );
    glProgramUniformMatrix4fv( shaderProgram, location, 1, GL_FALSE, value );
}

