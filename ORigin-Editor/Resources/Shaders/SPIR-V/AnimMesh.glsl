// type vertex
#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aUV;
layout(location = 4) in vec4 aBoneIDs;
layout(location = 5) in vec4 aBoneWeights;
layout(location = 6) in float aTexIndex;
layout(location = 7) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjection;
    vec3 Position;
} CameraBuffer;

layout(std140, binding = 1) uniform Model
{
    mat4 ModelMatrix;
    mat4 BoneTransforms[50];
} ModelBuffer;

struct Vertex
{
    vec3 Position;
    vec3 Normals;
    vec4 Color;
    vec2 UV;
};

layout(location = 0) out Vertex Out;
layout(location = 4) out flat float vTexIndex;
layout(location = 5) out flat int vEntityID;

void main()
{
    mat4 boneTransform = mat4(0.0);

    boneTransform += ModelBuffer.BoneTransforms[int(aBoneIDs.x)] * aBoneWeights.x;
    boneTransform += ModelBuffer.BoneTransforms[int(aBoneIDs.y)] * aBoneWeights.y;
    boneTransform += ModelBuffer.BoneTransforms[int(aBoneIDs.z)] * aBoneWeights.z;
    boneTransform += ModelBuffer.BoneTransforms[int(aBoneIDs.w)] * aBoneWeights.w;
    
    vec4 position = boneTransform * vec4(aPosition, 1.0);

    Out.Position = vec3(ModelBuffer.ModelMatrix * boneTransform * position);
    Out.Normals = mat3(transpose(inverse(ModelBuffer.ModelMatrix * boneTransform))) * aNormals;
    Out.Normals = normalize(Out.Normals);

    gl_Position = CameraBuffer.ViewProjection * position;

    Out.Color = aColor;
    Out.UV = aUV;
    vTexIndex = aTexIndex;
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

layout(std140, binding = 2) uniform Material
{
    float Emission;
    float Metallic;
    float Roughness;
    bool UseNormalMaps;
} MaterialBuffer;

layout(std140, binding = 3) uniform Lighting
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
	vec3 viewDirection = normalize(CameraBuffer.Position - In.Position);

    vec3 lighting = DirectionalLight(In.Normals, viewDirection);
    vec4 textureColor = texture(uTextures[int(vTexIndex)], In.UV);

    vec3 finalColor = (lighting * textureColor.rgb) * In.Color.rgb;
    float gamma = 2.2;
    finalColor = ApplyGammaCorrection(finalColor, gamma);
    oColor = vec4(finalColor, 1.0);

    oEntityID = vEntityID;
}
