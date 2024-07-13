// type vertex
#version 450 core
layout (std140, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	vec3 Position;
} Camera;

layout(std140, binding = 1) uniform ModelBuffer
{
	mat4 ModelTransform;
};

layout(std140, binding = 2) uniform LightingBuffer
{
	mat4 ViewProjection;
} Lighting;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec4 fragPositionLightSpace;

void main()
{
	fragPosition = vec3(ModelTransform * vec4(aPos, 1.0));
  fragNormal = mat3(transpose(inverse(ModelTransform))) * aNormal;
	fragTexCoord = aTexCoord;

	fragPositionLightSpace = Lighting.ViewProjection * vec4(fragPosition, 1.0);

	vec4 cameraSpacePosition = Camera.ViewProjection * ModelTransform * vec4(aPos, 1.0);
	gl_Position = cameraSpacePosition;
}

// type fragment
#version 450 core
layout (std140, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	vec3 Position;
} Camera;

layout(std140, binding = 1) uniform ModelBuffer
{
	mat4 ModelTransform;
	int EntityID;
};

layout(std140, binding = 3) uniform DirectionalLightBuffer
{
  vec4 Direction;
  vec4 Color;
  float Strength;
	float Diffuse;
	float Specular;
} Dirlight;

layout(std140, binding = 4) uniform MaterialBuffer
{
	vec4 Color;
	float Emission;
	float Metallic;
	float Roughness;
	bool UseNormalMaps;
} Material;

layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec4 fragPositionLightSpace;

layout(binding = 0) uniform sampler2D u_AlbedoMap;
layout(binding = 1) uniform sampler2D u_SpecularMap;
layout(binding = 2) uniform sampler2D u_ShadowMap;

float calcShadow(vec3 normal, vec3 lightDirection);
vec3 calculateDirectionalLight(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture);

void main()
{
	vec3 normal = normalize(fragNormal);
	vec3 viewDirection = normalize(fragPosition - Camera.Position);

	vec3 diffuseTexture = texture(u_AlbedoMap, fragTexCoord).rgb;
	vec3 specularTexture = texture(u_SpecularMap, fragTexCoord).rgb;
	vec3 totalLight = calculateDirectionalLight(normal, viewDirection, diffuseTexture, specularTexture);

	vec3 finalColor = totalLight * Material.Color.rgb;

	oColor = vec4(1.0, 0.0, 0.0, 1.0);
	oEntityID = EntityID;
}

float calcShadow(vec3 normal, vec3 lightDirection)
{
  vec3 projCoords = fragPositionLightSpace.xyz / fragPositionLightSpace.w;
  projCoords = projCoords * 0.5 + 0.5;
	if(projCoords.z > 1.0)
		return 0.0;

	float shadow = 0.0;

	float bias = max(0.0005 * (1.0 - dot(normal, lightDirection)), 0.00005);
  float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
  float currentDepth = projCoords.z;

	vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
	int rep = 1;
	for(int x = -rep; x <= rep; ++x)
	{
		for(int y = -rep; y <= rep; ++y)
		{
			float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow = currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;
	return shadow;
}

vec3 calculateDirectionalLight(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture)
{
	vec3 lightDirection = normalize(Dirlight.Direction.xyz);
	vec3 strength = Dirlight.Strength * Dirlight.Color.rgb * diffuseTexture;
	float diffuseFactor = max(dot(lightDirection, normal), 0.0);
	vec3 diffuseColor = diffuseFactor * Dirlight.Diffuse * Dirlight.Color.rgb * diffuseTexture;
	vec3 halfwayDir = normalize(lightDirection + viewDirection);  
	float specularFactor = pow(max(dot(viewDirection, halfwayDir), 0.0), 64.0);
	vec3 specularColor = specularFactor * Dirlight.Specular * Dirlight.Color.rgb * specularTexture;
	float shadow = calcShadow(normal, lightDirection);
	return (strength + (1.0 - shadow) * (diffuseColor + specularColor));
}