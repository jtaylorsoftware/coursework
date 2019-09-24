#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 position;

uniform mat4 uRenderProjection;
uniform mat4 uRenderView;

void main()
{
    position = aPos;
    gl_Position = vec4(uRenderProjection * uRenderView * vec4(aPos, 1.0)).xyww;
}  