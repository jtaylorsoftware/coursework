#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 uLightSpaceMat;
uniform mat4 uModel;

void main(){
	gl_Position = uLightSpaceMat * uModel * vec4(aPos, 1.0); // transform into light space
	// depth will be written out to a texture and used for shadow map calculations
	// in other shaders
}