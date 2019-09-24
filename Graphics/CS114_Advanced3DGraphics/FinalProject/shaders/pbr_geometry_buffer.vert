#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out VS_OUT
{
    mat3 TBN;
	vec4 lightSpacePos;
    vec3 worldPos;
    vec2 uv;
} vs_out;

uniform mat4 uModel;
uniform mat4 uLightSpaceMat;
uniform mat3 uNormalMatrix;

layout(binding=0) uniform Matrices
{
    mat4 projection;    
    mat4 view;             
}; 

void main()
{

    vec3 T = normalize(uNormalMatrix * aTangent);    // tangent 
    vec3 N = normalize(uNormalMatrix * aNormal);     // normal
    T = normalize(T - dot(T, N) * N);               // use gramm-schmidt process to re-ortho the TBN
    vec3 B = cross(N, T);                           // bitangent
    
    vs_out.TBN = mat3(T, B, N); // tangent to world space
    
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vs_out.worldPos = worldPos.xyz; 
    vs_out.uv = aTexCoords;
	vs_out.lightSpacePos = uLightSpaceMat * vec4(vs_out.worldPos, 1.0);
    gl_Position = projection * view * worldPos;
}