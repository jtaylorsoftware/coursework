#version 450 core

layout(binding = 0) uniform sampler2D diffuseMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D roughnessMap;
layout(binding = 3) uniform sampler2D metallicMap;
layout(binding = 4) uniform sampler2D aoMap;
layout(binding = 5) uniform sampler2D heightMap;
layout(binding = 9) uniform sampler2D dirLightShadowMap;

in VS_OUT
{
    mat3 TBN;
	vec4 lightSpacePos;
    vec3 worldPos;
    vec2 uv;
} fs_in;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outAlbedoSpec;

uniform vec3 dirLightDirection;

void main()
{   

    // obtain normal from normal map in range [0,1] // 0->1 for rgb components 0/255 -> 255/255
	vec4 normalSample = texture(normalMap, fs_in.uv);
    vec3 normal = normalSample.rgb;
	
    // transform normal vector to range [-1,1] // map 0 to -1, 255 to 1
    normal = normalize(normal.rgb * 2.0 - 1.0);  
    normal = normalize(fs_in.TBN * normal.rgb); // this is in world space
	
	outNormal.rgb	 = normal;
	outNormal.a = texture(metallicMap, fs_in.uv).r;
	
    outPosition = vec4(fs_in.worldPos, 1.0);
	
	// calculate if in shadow
	vec3 projected = fs_in.lightSpacePos.xyz / fs_in.lightSpacePos.w;
	projected = projected * 0.5 + 0.5;
	
	float closestDepth = texture(dirLightShadowMap, projected.xy).r;
	float currentDepth = projected.z;
	
	float shadowBias = max(0.005 * (1.0 - dot(normal, -dirLightDirection)), 0.00005);
	float inShadow = (currentDepth - shadowBias) > closestDepth ? 1.0 : 0.0;
    outPosition.a = inShadow;
	
	
    outAlbedoSpec.rgb = texture(diffuseMap, fs_in.uv).rgb;
    outAlbedoSpec.a = texture(roughnessMap, fs_in.uv).r;
}  