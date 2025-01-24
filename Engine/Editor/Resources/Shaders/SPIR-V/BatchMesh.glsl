// type vertex
#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aUV;
layout(location = 4) in vec2 aTilingFactor;
layout(location = 5) in float aAlbedoIndex;
layout(location = 6) in float aSpecularIndex;

layout(std140, binding = 0) uniform Camera
{
    mat4 view_projection;
    vec3 position;
} CameraBuffer;

layout(std140, binding = 1) uniform Lighting
{
    mat4 LightSpaceMatrix;
    vec4 Position;
    vec4 Color;
    vec4 Ambient;
    float Diffuse;
    float Specular;
} LightBuffer;

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

void main()
{
    gl_Position = CameraBuffer.ViewProjection * vec4(aPosition, 1.0);
    Out.Position = aPosition;
    Out.Normals = aNormals;
    Out.Color = aColor;
    Out.UV = aUV;
    Out.TilingFactor = aTilingFactor;
    vAlbedoIndex = aAlbedoIndex;
    vSpecularIndex = aSpecularIndex;
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;

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

layout(binding = 0) uniform sampler2D uTextures[32];

layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjection;
    vec3 Position;
} CameraBuffer;

layout(std140, binding = 1) uniform Lighting
{
    mat4 LightSpaceMatrix;
    vec4 Position;
    vec4 Color;
    vec4 Ambient;
    float Diffuse;
    float Specular;
} LightBuffer;

layout(std140, binding = 2) uniform Material
{
    float Emission;
    float Metallic;
    float Roughness;
    bool UseNormalMaps;
} MaterialBuffer;

float ShadowCalculation()
{
    vec4 fragPosLightSpace = LightBuffer.LightSpaceMatrix * vec4(In.Position, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0)
        return 0.0;
    float bias = max(0.005 * (1.0 - dot(normalize(In.Normals), normalize(LightBuffer.Position.xyz - In.Position))), 0.005);
    float shadow = 0.0;
    float offset = 1.0 / 2048.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uTextures[1], projCoords.xy + vec2(x, y) * offset).r;
            shadow += (projCoords.z - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main()
{
    vec3 color = texture(uTextures[int(vAlbedoIndex)], In.UV * In.TilingFactor).rgb;
    vec3 normal = normalize(In.Normals);
    vec3 lightColor = LightBuffer.Color.rgb;
    vec3 ambient = LightBuffer.Ambient.rgb * lightColor;
    vec3 lightDir = normalize(LightBuffer.Position.xyz - In.Position);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(CameraBuffer.Position - In.Position);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor * LightBuffer.Specular;
    float shadow = ShadowCalculation();
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    oColor = vec4(lighting, 1.0) * In.Color;
}
