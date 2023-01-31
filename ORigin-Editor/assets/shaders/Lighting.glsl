// type vertex
#version 450 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in float aIntensity;
layout (location = 3) in int aEntityID;

layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
};

layout (std140, binding = 1) uniform Light
{
	vec3 lightPosition;
	vec4 lightColor;
	float lightIntensity;
};

struct Vertex
{
	vec4 LightColor;
	float Intensity;
};

layout (location = 0) out Vertex vertexOut;
layout (location = 2) out flat int vEntityID;

void main()
{

    gl_Position = ViewProjection * vec4(aPosition, 1.0f);

	vertexOut.LightColor = aColor;
	vertexOut.Intensity = aIntensity;
	vEntityID = aEntityID;
}

// type fragment
#version 450 core
layout (location = 0) out vec4 Color;
layout (location = 1) out int EntityID;

layout (std140, binding = 1) uniform Light
{
	vec3 lightPosition;
	vec4 lightColor;
	float lightIntensity;
};

struct Vertex
{
	vec4 LightColor;
	float Intensity;
};

layout (location = 0) in Vertex vertexInput;
layout (location = 2) in flat int vEntityID;

void main()
{
	Color = vertexInput.LightColor * vertexInput.Intensity;
	EntityID = vEntityID;
}
