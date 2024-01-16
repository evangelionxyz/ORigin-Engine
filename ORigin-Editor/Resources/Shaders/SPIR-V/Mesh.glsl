// type vertex
#version 450 core
layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec3 Position;

} CameraBuffer;

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

layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec2 vTexCoord;
layout(location = 3) out vec4 vLightPosition;

void main()
{
	vPosition = vec3(ModelTransform * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(ModelTransform))) * aNormal;
	vTexCoord = aTexCoord;

	vLightPosition = Lighting.ViewProjection * vec4(vPosition, 1.0);
	gl_Position = CameraBuffer.ViewProjection * ModelTransform * vec4(aPos, 1.0);
}

// type fragment
#version 450 core
layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec3 Position;

} CameraBuffer;

layout(std140, binding = 1) uniform ModelBuffer
{
	mat4 ModelTransform;
	int EntityID;
};

layout(std140, binding = 3) uniform DirectionalLightBuffer
{
    vec4 Direction;
    vec4 Color;
    float Ambient;
    float Diffuse;
    float Specular;
} DirLight;

layout(std140, binding = 4) uniform MaterialBuffer
{
	vec4 Color;
	vec2 TilingFactor;
} Material;

layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec4 vLightPosition;

layout(binding = 0) uniform sampler2D u_DiffTexture;
layout(binding = 1) uniform sampler2D u_SpecTexture;
layout(binding = 2) uniform sampler2D u_ShadowMap;

vec3 AddDirectionalLight(vec3 diffuseTexture, vec3 specularTexture, vec3 normal, vec3 viewDirection);
float ShadowCalculation();

void main()
{
	vec3 norm = normalize(vNormal);
	vec3 viewDirection = normalize(CameraBuffer.Position - vPosition);

	vec3 diffTexture = texture(u_DiffTexture, vec2(vTexCoord * Material.TilingFactor)).rgb;
	vec3 specTexture = texture(u_SpecTexture, vec2(vTexCoord * Material.TilingFactor)).rgb;
    diffTexture = vec3(1.0);
    specTexture = vec3(1.0);

	vec3 lights = vec3(0.0);

	lights += AddDirectionalLight(diffTexture, specTexture, norm, viewDirection);
	oColor = vec4(lights, 1.0) * Material.Color;

	oEntityID = EntityID;
}

vec3 AddDirectionalLight(vec3 diffuseTexture, vec3 specularTexture, vec3 normal, vec3 viewDirection)
{
	vec3 lightDirection = normalize(DirLight.Direction.xyz);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    vec3 ambient = DirLight.Ambient * DirLight.Color.xyz * diffuseTexture;

    float diffuseContrib = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diffuseContrib * DirLight.Diffuse * DirLight.Color.xyz * diffuseTexture;
    
    float specularContrib = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    vec3 specular = DirLight.Specular * specularContrib * DirLight.Color.xyz; //* specularTexture;

    float shadow = ShadowCalculation();
    return ambient + (1.0 - shadow) * (diffuse + specular);
}

float ShadowCalculation() {
    vec3 projectionCoords = vLightPosition.xyz / vLightPosition.w;
    projectionCoords = projectionCoords * 0.5 + 0.5;

    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(DirLight.Direction.xyz - vPosition);

    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.000000005);
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    float shadow = 0.0;

    float currentDepth = projectionCoords.z;
    if(currentDepth > 1.0)
        currentDepth = 1.0;

    int rep = 1;
    for (int x = -rep; x <= rep; x++)
    {
        for (int y = -rep; y <= rep; y++)
        {
            vec2 offset = vec2(x, y) * texelSize;
            float pcfDepth = texture(u_ShadowMap, projectionCoords.xy + offset).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}