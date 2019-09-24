#version 450 core
out vec4 fragColor;

in vec2 texCoords;

layout (binding = 0) uniform sampler2D lightPassColor;

void main()
{
	// Just perform gamma correction on color
    vec3 lightPassColor = texture(lightPassColor, texCoords).rgb;
	fragColor = vec4(pow(lightPassColor, vec3(1.0/2.2)), 1.0); 
} 