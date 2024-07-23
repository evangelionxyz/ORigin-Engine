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

layout(std140, binding = 1) uniform Light
{
    vec4 Direction;
    vec4 Color;
    vec4 AmbientColor;
} LightBuffer;

void main()
{
    // Compute the normal
    vec3 normal = normalize(In.Normal);
    vec3 lightDir = normalize(-LightBuffer.Direction.xyz);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LightBuffer.Color.rgb;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * LightBuffer.AmbientColor.rgb * LightBuffer.Color.rgb;

    vec3 result = (ambient + diffuse) * LightBuffer.Color.rgb;

    vec4 textureColor = texture(uTextures[int(vTexIndex)], In.TexCoord);
    oColor = vec4(result, 1.0) * textureColor * In.Color;

    oEntityID = vEntityID;
}
