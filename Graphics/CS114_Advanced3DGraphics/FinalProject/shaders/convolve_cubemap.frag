#version 450 core

layout(binding = 0) uniform samplerCube skybox;

in vec3 position;
out vec4 fragColor;

const float PI = 3.14159265359;

// See http://www.codinglabs.net/article_physically_based_rendering.aspx
// The code below is similar, but translated to OpenGL.
// See also https://learnopengl.com/PBR/Theory
void main()
{    
	// hemisphere is centered around its surface normal (thus normal is normalized position)
    vec3 normal = normalize(position);
	vec3 irradiance = vec3(0.0);
	
	vec3 upDir = vec3(0.0, 1.0, 0.0);
	vec3 rightDir = cross(upDir, normal);
	upDir = cross(normal, rightDir);
	
	float sampleDelta = 0.025;
	float numSamples = 0.0;
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta){
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta){
			vec3 tangentSample = vec3(
				sin(theta) * cos(phi), 
				sin(theta) * sin(phi),
				cos(theta));
		
			vec3 worldSample = tangentSample.x * rightDir + tangentSample.y * upDir + tangentSample.z * normal;
			
			irradiance += texture(skybox, worldSample).rgb * cos(theta) * sin(theta);
			++numSamples;
		}	
	}
	irradiance = PI * irradiance * (1.0 / numSamples);
	fragColor = vec4(irradiance, 1.0);
}