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
	int EntityID;
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
	vec2 TilingFactor;
	float Metallic;
	float Roughness;
} Material;

layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec4 fragPositionLightSpace;

layout(binding = 0) uniform sampler2D u_DiffTexture;
layout(binding = 1) uniform sampler2D u_SpecTexture;
layout(binding = 2) uniform sampler2D u_ShadowMap;
layout(binding = 3) uniform sampler2D u_HDRColor;

float calcShadow(vec3 normal, vec3 lightDirection);
vec3 calculateDirectionalLight(vec3 normal, vec3 viewDirection, vec3 diffTexture, vec3 specTexture);

void main()
{
	vec3 normal = normalize(fragNormal);
	vec3 viewDirection = normalize(fragPosition - Camera.Position);

	vec3 diffTexture = texture(u_DiffTexture, vec2(fragTexCoord * Material.TilingFactor)).rgb;
	vec3 specTexture = texture(u_SpecTexture, vec2(fragTexCoord * Material.TilingFactor)).rgb;
	vec3 totalLight = calculateDirectionalLight(normal, viewDirection, diffTexture, specTexture);
	vec3 finalColor = totalLight * Material.Color.rgb;

	float gamma = 2.2;
	//finalColor *= pow(finalColor, vec3(1.0/gamma));
	oColor = vec4(finalColor, 1.0);

	vec3 result = vec3(1.0) - exp(-finalColor * 2.0);
  result = pow(result, vec3(1.0 / gamma));
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

vec3 calculateDirectionalLight(vec3 normal, vec3 viewDirection, vec3 diffTexture, vec3 specTexture)
{
	vec3 lightDirection = normalize(Dirlight.Direction.xyz);

	vec3 strength = Dirlight.Strength * Dirlight.Color.rgb * diffTexture;
	float diffuseFactor = max(dot(lightDirection, normal), 0.0);
	vec3 diffuseColor = diffuseFactor * Dirlight.Diffuse * Dirlight.Color.rgb * diffTexture;

	//vec3 reflectDirection = reflect(-lightDirection, normal);
	//float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), 64.0);
	vec3 halfwayDir = normalize(lightDirection + viewDirection);  
	float specularFactor = pow(max(dot(viewDirection, halfwayDir), 0.0), 64.0);
	vec3 specularColor = specularFactor * Dirlight.Specular * Dirlight.Color.rgb * specTexture;

	float shadow = calcShadow(normal, lightDirection);

  return strength + (1.0 - shadow) * (diffuseColor + specularColor);
}