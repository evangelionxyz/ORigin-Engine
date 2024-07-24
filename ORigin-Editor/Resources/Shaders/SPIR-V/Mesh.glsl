// type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in float aTexIndex;
layout(location = 5) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjection;
    vec3 Position;
} CameraBuffer;

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec4 Color;
    vec2 TexCoord;
};

layout(location = 0) out Vertex Out;
layout(location = 4) out flat float vTexIndex;
layout(location = 5) out flat int vEntityID;

void main()
{
    Out.Position = aPosition;
    Out.Normal = aNormal;
    Out.Color = aColor;
    Out.TexCoord = aTexCoord;

    vTexIndex = aTexIndex;
    vEntityID = aEntityID;

    gl_Position = CameraBuffer.ViewProjection * vec4(aPosition, 1.0);
}


// type fragment
#version 450 core

layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec4 Color;
    vec2 TexCoord;
};

layout(location = 0) in Vertex In;
layout(location = 4) in flat float vTexIndex;
layout(location = 5) in flat int vEntityID;

layout(binding = 0) uniform sampler2D uTextures[32];

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


vec3 DirectionalLight(vec3 normal, vec3 viewDirection)
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
    vec3 specular = specularFactor * LightingBuffer.Specular * LightingBuffer.Color.rgb;

    return ambient + diffuse + specular;
}

vec3 ApplyGammaCorrection(vec3 color, float gamma)
{
    return pow(color, vec3(1.0 / gamma));
}

void main()
{
    // Compute the normal
    vec3 normal = normalize(In.Normal);
	vec3 viewDirection = normalize(CameraBuffer.Position - In.Position);

    vec3 lighting = DirectionalLight(normal, viewDirection);
    vec4 textureColor = texture(uTextures[int(vTexIndex)], In.TexCoord);

    vec3 finalColor = (lighting * textureColor.rgb) * In.Color.rgb;
    float gamma = 2.2;
    finalColor = ApplyGammaCorrection(finalColor, gamma);
    oColor = vec4(finalColor, 1.0);

    oEntityID = vEntityID;
}
