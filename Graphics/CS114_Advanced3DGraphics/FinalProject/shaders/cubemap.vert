#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

layout(binding=0) uniform Matrices
{
    mat4 projection;    
    mat4 view;          
}; 

void main()
{
    texCoords = aPos;
    gl_Position = vec4(projection * mat4(mat3(view)) * vec4(aPos, 1.0)).xyww;
}  