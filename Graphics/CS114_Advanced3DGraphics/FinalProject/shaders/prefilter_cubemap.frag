#version 450 core

layout(binding = 0) uniform samplerCube uSkybox;

uniform float uRoughness;

in vec3 position;
out vec4 fragColor;

const float PI = 3.14159265359;

float TrowbridgeReitzGGXNDF(vec3 n , vec3 h, float roughness);
float RadicalInverse_Vdc(uint bits);
vec2 Hammersley(uint i, uint n);
vec3 ImportanceSampleGGX(vec2 xi, vec3 n, float roughness);

// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
void main()
{    
	// Approximate possible view directions by assuming the view direction (and the reflection direction)
	// is always equal to the output sample direction 
    vec3 normal = normalize(position);
	
	vec3 reflectDir = normal;
	vec3 viewDir = reflectDir;
	
	vec3 prefilterColor = vec3(0.0);
	float totalWeight = 0.0;
	
	const uint SAMPLES = 1024u;
	for(uint i = 0u; i < SAMPLES; ++i){
		vec2 xi = Hammersley(i, SAMPLES);
		vec3 h = ImportanceSampleGGX(xi, normal, uRoughness);
		vec3 l = normalize(2.0 * dot(viewDir, h)  * h - viewDir);
		
		float nDotL = max(dot(normal, l), 0.0);
		if(nDotL > 0.0){
		
			// https://chetanjags.wordpress.com/2015/08/26/image-based-lighting/
			// Fixes bright dots in the convolution due to high frequency details and varying
			// light intensity.
			float d = TrowbridgeReitzGGXNDF(normal, h, uRoughness);
			float nDotH = max(dot(normal, h), 0.0);
			float hDotV = max(dot(h, viewDir), 0.0);
			float pdf = d * nDotH / (4.0 * hDotV) + 0.0001;
			
			float resolution = 512.0;
			float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
			float saSample = 1.0 / (float(SAMPLES) * pdf + 0.0001);
			
			float mipmapLevel = (uRoughness == 0.0) ? 0.0 : 0.5 * log2(saSample/saTexel);
		
			// ----
			
			prefilterColor += textureLod(uSkybox, l, mipmapLevel).rgb * nDotL;
			totalWeight += nDotL;
		}
	}
	
	prefilterColor *= (1.0 / totalWeight);
	
	fragColor = vec4(prefilterColor, 1.0);
}


// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// BRDF equations translated directly to code with no optimizations
// 
float TrowbridgeReitzGGXNDF(vec3 n , vec3 h, float roughness){
	float roughSq = roughness * roughness;
	float nDotH = max(dot(n,h), 0.0);
	float nDotHSq = nDotH * nDotH;
	
	float denom = (nDotHSq * (roughSq - 1.0) + 1.0);
	return roughSq / (PI * denom * denom);
}


// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// Efficiently generates random hemisphere directions using low discrepancy
// sequences (quasi Monte Carlo method)
float RadicalInverse_Vdc(uint bits){
	 bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // 2.3283064365386963e-10 = 0x100000000
}
vec2 Hammersley(uint i, uint n){
	return vec2(float(i)/float(n), RadicalInverse_Vdc(i));
}


vec3 ImportanceSampleGGX(vec2 xi, vec3 n, float roughness){
	// Orient and bias a sample vector towards specular lobe of a surface with some amount
	// of roughness 
	
	// NDF, generated direction using Xi and biased with roughness 
	float roughSq = roughness * roughness;
	float phi = 2.0 * PI * xi.x;
	float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (roughSq*roughSq - 1.0) * xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
	// spherical to cartesian
	vec3 h = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
	
	// local space conversion 
	vec3 upDir = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(upDir, n));
	vec3 bitangent = cross(n, tangent);
	
	return normalize(tangent * h.x + bitangent * h.y + n * h.z);
}
