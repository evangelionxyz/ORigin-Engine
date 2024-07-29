// type vertex
#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aUV;
layout(location = 4) in vec2 aTilingFactor;
layout(location = 5) in vec4 aBoneIDs;
layout(location = 6) in vec4 aBoneWeights;
layout(location = 7) in float aAlbedoIndex;
layout(location = 8) in float aSpecularIndex;
layout(location = 9) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjection;
    vec3 Position;
} CameraBuffer;

struct Vertex
{
    vec3 Position;
    vec3 Normals;
    vec4 Color;
    vec2 UV;
    vec2 TilingFactor;
};

layout(location = 0) out Vertex Out;
layout(location = 5) out flat float vAlbedoIndex;
layout(location = 6) out flat float vSpecularIndex;
layout(location = 7) out flat int vEntityID;

void main()
{
    gl_Position = CameraBuffer.ViewProjection * vec4(aPosition, 1.0);

    Out.Position = aPosition;
    Out.Normals = normalize(aNormals);
    Out.Color = aColor;
    Out.UV = aUV;
    Out.TilingFactor = aTilingFactor;
    vAlbedoIndex = aAlbedoIndex;
    vSpecularIndex = aSpecularIndex;
    vEntityID = aEntityID;
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

struct Vertex
{
    vec3 Position;
    vec3 Normals;
    vec4 Color;
    vec2 UV;
    vec2 TilingFactor;
};

layout(location = 0) in Vertex In;
layout(location = 5) in flat float vAlbedoIndex;
layout(location = 6) in flat float vSpecularIndex;
layout(location = 7) in flat int vEntityID;

layout(binding = 0) uniform sampler2D uAlbedoTextures[16];
layout(binding = 1) uniform sampler2D uSpecularTextures[16];

layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjection;
    vec3 Position;
} CameraBuffer;

layout(std140, binding = 1) uniform Material
{
    float Emission;
    float Metallic;
    float Roughness;
    bool UseNormalMaps;
} MaterialBuffer;

layout(std140, binding = 2) uniform Lighting
{
    vec4 Direction;
    vec4 Color;
    vec4 Ambient;
    float Diffuse;
    float Specular;
} LightingBuffer;

vec3 DirectionalLight(vec3 normal, vec3 viewDirection, vec3 specularTexture)
{
    vec3 lightDirection = normalize(LightingBuffer.Direction.xyz);
    
    // Ambient component
    vec3 ambient = LightingBuffer.Ambient.rgb;

    // Diffuse component
    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diffuseFactor * LightingBuffer.Diffuse * LightingBuffer.Color.rgb;

    // Specular component
    vec3 halfwayDir = normalize(lightDirection + viewDirection);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 32.0); // Shininess factor can be adjusted
    vec3 specular = specularFactor * LightingBuffer.Specular * specularTexture * LightingBuffer.Color.rgb;

    return ambient + diffuse + specular;
}

vec3 ApplyGammaCorrection(vec3 color, float gamma)
{
    return pow(color, vec3(1.0 / gamma));
}

void main()
{
    // Compute the normal
	vec3 viewDirection = normalize(CameraBuffer.Position - In.Position);
    vec3 specularTexture = texture(uSpecularTextures[int(vSpecularIndex)], In.UV * In.TilingFactor).rgb;
    vec3 textureColor = texture(uAlbedoTextures[int(vAlbedoIndex)], In.UV * In.TilingFactor).rgb;

    vec3 lighting = DirectionalLight(In.Normals, viewDirection, specularTexture);

    vec3 finalColor = (lighting * textureColor) * In.Color.rgb;
    float gamma = 2.2;
    //finalColor = ApplyGammaCorrection(finalColor, gamma);
    oColor = vec4(finalColor, 1.0);

    oEntityID = vEntityID;
}
