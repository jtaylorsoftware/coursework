#version 450 core
out vec2 FragColor;
in vec2 texCoords;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
vec2 IntegrateBRDF(float NdotV, float roughness);


float SchlickGGXGeometry(float dotTerm, float roughness);
float SmithGeometry(vec3 n, vec3 v, vec3 l, float roughness);

// Creates a BRDF lookup texture using methods described by Epic Games.
// See http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// for Epic Games' original paper.
// https://learnopengl.com/PBR/Theory breaks down a lot of the math explanations behind it. 
void main() 
{
    FragColor = IntegrateBRDF(texCoords.x, texCoords.y);;
}


// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// Efficiently generates random hemisphere directions using low discrepancy
// sequences (quasi Monte Carlo method)
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}


vec3 ImportanceSampleGGX(vec2 xi, vec3 n, float roughness)
{
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

// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// BRDF equations translated directly to code with no optimizations
float SchlickGGXGeometry(float dotTerm, float roughness)
{
    float k = (roughness * roughness) / 2.0;

    float nom   = dotTerm;
    float denom = dotTerm * (1.0 - k) + k;

    return nom / denom;
}

float SmithGeometry(vec3 n, vec3 v, vec3 l, float roughness)
{
    float NdotV = max(dot(n, v), 0.0);
    float NdotL = max(dot(n, l), 0.0);
    float ggx2 = SchlickGGXGeometry(NdotV, roughness);
    float ggx1 = SchlickGGXGeometry(NdotL, roughness);

    return ggx1 * ggx2;
}

// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
vec2 IntegrateBRDF(float NdotV, float roughness)
{
	// Output a scale and bias for the specular BRDF term 
	// for a specified number of samples

    vec3 V;
    V.x = sqrt(1.0 - NdotV*NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0; 

    vec3 N = vec3(0.0, 0.0, 1.0);
    
    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float G = SmithGeometry(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}