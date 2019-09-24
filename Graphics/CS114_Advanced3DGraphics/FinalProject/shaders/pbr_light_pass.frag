#version 450 core

layout(location = 2) in vec2 texCoords;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D uPosition;
layout(binding = 1) uniform sampler2D uNormal;
layout(binding = 2) uniform sampler2D uAlbedoSpec;
layout(binding = 3) uniform samplerCube uIrradianceMap;
layout(binding = 4) uniform samplerCube uPrefilterMap;
layout(binding = 5) uniform sampler2D uBrdfLut;

layout(location = 0) uniform vec3 uViewPos;

// GLSL Light structures to match C++ structs 
struct DirectionalLight // size 8N = 32
{
    vec4 direction; 
    vec4 color;
};
/*
struct PointLight // size 8N = 32
{
    vec4 position;
    vec4 color;
};
*/
layout(std140, binding=1) uniform Lights
{
    DirectionalLight dirLightSrc;
};


/*
vec3 CalculatePointLight(int lightIndex, vec3 worldPos, vec3 albedo, 
                         vec3 normal, float roughness, float metallic, vec3 F0, vec3 viewDirection);
*/                       
vec3 CalculateDirLight(vec3 albedo, vec3 normal, float roughness, float metallic,
					   vec3 F0, vec3 viewDirection, float inShadow);

vec3 SchlickFresnel(vec3 v, vec3 h, vec3 F0, float roughness);
float TrowbridgeReitzGGXNDF(vec3 n , vec3 h, float roughness);
float SchlickGGXGeometry(float dotTerm, float roughness);
float SmithGeometry(vec3 n, vec3 v, vec3 l, float roughness);

// Doesn't seem to give nice results (namely point lights seemed to ignore normals, view direction for some angles)
// but that may have been an issue with the normal maps I used
//float CookTorranceGeometry(vec3 n, vec3 h, vec3 l, vec3 v);

#define PI 3.14159265359

void main()
{
    vec4 posSample = texture(uPosition, texCoords);
	vec3 worldPos = posSample.rgb;
	float inShadow = posSample.a;
	
    vec4 normalSample = texture(uNormal, texCoords);
	vec3 normal = normalSample.rgb;
	float metallic = normalSample.a;
	
	vec4 albedoSpecSample = texture(uAlbedoSpec, texCoords).rgba;
    vec3 albedo = albedoSpecSample.rgb;
    float roughness = albedoSpecSample.a;
    
    vec3 viewDirection = normalize(uViewPos - worldPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	// Point lighting is not used in the final version for the project
	// (it will work if lights are uploaded to shader in the uniform block,
	//  but not necessary to show nice specular highlights when there's IBL)
    vec3 pointLighting = vec3(0, 0, 0);
    //for (int i = 0; i < NUM_PT_LIGHTS; ++i)
    //{
        //pointLighting += CalculatePointLight(i, worldPos, albedo, normal, roughness, metallic, F0, viewDirection);
    //}
    
    vec3 dirLight = CalculateDirLight(albedo, normal, roughness, metallic, F0, viewDirection, inShadow);
    //vec3 dirLight = vec3(0);

	//vec3 ambient = vec3(0.2) * albedo;
	vec3 Ks = SchlickFresnel(normal, viewDirection, F0, roughness);
	vec3 Kd = 1.0 - Ks;
	Kd *= 1.0 - metallic;
	vec3 irradiance = (texture(uIrradianceMap, normal).rgb);
	vec3 diffuse = irradiance * albedo;
	
	vec3 reflectDir = reflect(-viewDirection, normal);
	vec3 prefilterColor = textureLod(uPrefilterMap, reflectDir, roughness * 4.0).rgb;
	vec2 brdf = texture(uBrdfLut, vec2(max(dot(normal, viewDirection), 0.0), roughness)).rg;
	vec3 specular = prefilterColor * (Ks * brdf.x + brdf.y);
	
	vec3 ambient = (Kd * diffuse + specular);
	
    // Resulting lighting
	vec3 color = dirLight + pointLighting + ambient;
    fragColor = vec4( color, 1.0);
}


vec3 CalculateDirLight(vec3 albedo, vec3 normal, float roughness, float metallic, 
                       vec3 F0, vec3 viewDirection, float inShadow)
{
    vec3 lightDirection = normalize(-dirLightSrc.direction.xyz);
    vec3 halfLight = normalize(lightDirection + viewDirection);
	
	// Calculate D, G, F of BRDF function
	float ndf = TrowbridgeReitzGGXNDF(normal, halfLight, roughness);
	float g = SmithGeometry(normal, viewDirection, lightDirection, roughness);
	vec3 f = SchlickFresnel(viewDirection, halfLight, F0, roughness);
	
	// Energy conservation: Kd = 1 - Ks, weighted by how metallic the surface is 
	vec3 Ks = f;
	vec3 Kd = (vec3(1.0) - Ks) * (1.0 - metallic);
	
	// BRDF equation (D*G*F)/(4*n.v*n.l)
	vec3 nominator = ndf * g * f;
	float denominator = 4 * max(dot(normal, lightDirection), 0.0) * max(dot(normal, viewDirection), 0.0) + 0.001;
    
	
	vec3 fr = Kd * albedo/PI; // lambertian diffuse 
	fr += nominator/denominator; // add in the specular BRDF 
	
	float nDotL = max(dot(normal, lightDirection), 0.0);
	
	// calculate final total irradiance 
	vec3 radiance = dirLightSrc.color.rgb;
	fr = radiance * (1.0 - inShadow) * (fr * nDotL);
	
	return fr;
}

/*
vec3 CalculatePointLight(int lightIndex, vec3 worldPos, vec3 albedo, 
                         vec3 normal, float roughness, float metallic, 
						 vec3 F0, vec3 viewDirection)
{
    vec3 lightDirection = normalize(pointLights[lightIndex].position.xyz - worldPos);
    vec3 halfLight = normalize(lightDirection + viewDirection);
	float distance = length(pointLights[lightIndex].position.xyz - worldPos);
	float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
	
	float ndf = TrowbridgeReitzGGXNDF(normal, halfLight, roughness);
	float g = SmithGeometry(normal, viewDirection, lightDirection, roughness);
	vec3 f = SchlickFresnel(viewDirection, halfLight, F0, roughness);
	
	vec3 Ks = f;
	vec3 Kd = (vec3(1.0) - Ks) * (1.0 - metallic);
	
	vec3 nominator = ndf * g * f;
	float denominator = 4 * max(dot(normal, lightDirection), 0.0) * max(dot(normal, viewDirection), 0.0) + 0.001;
    
	vec3 fr = Kd * albedo/PI;
	fr += nominator/denominator;
	
	float nDotL = max(dot(normal, lightDirection), 0.0);
	
	vec3 radiance = pointLights[lightIndex].color.rgb * attenuation;
	
	fr = radiance * (fr * nDotL);
	
	return fr;
}
*/

// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// BRDF equations translated directly to code with no optimizations
// See also http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
vec3 SchlickFresnel(vec3 v, vec3 h, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0-roughness), F0) - F0) * pow(1.0 - max(dot(v, h), 0.0), 5.0);
}

float TrowbridgeReitzGGXNDF(vec3 n , vec3 h, float roughness){
	float roughSq = roughness * roughness;
	float nDotH = max(dot(n,h), 0.0);
	float nDotHSq = nDotH * nDotH;
	
	float denom = (nDotHSq * (roughSq - 1.0) + 1.0);
	return roughSq / (PI * denom * denom);
}

/*
float CookTorranceGeometry(vec3 n, vec3 h, vec3 l, vec3 v){
	float nDotH = max(dot(n, h), 0.0);
	float nDotV = max(dot(n, v), 0.0);
	float nDotL = max(dot(n, l), 0.0);
	float vDotH = max(dot(v, h), 0.0);
	
	float nDotVTerm = (2 * nDotH * nDotV)/vDotH;
	float nDotLTerm = (2 * nDotH * nDotL)/vDotH;
	
	float minTerm = min(nDotVTerm, nDotLTerm);
	return min(1.0, minTerm);
}
*/

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

