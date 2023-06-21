// type vertex
#version 460 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aColor;
layout(location = 4) in int aEntityID;

layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
} cameraBuffer;

layout(std140, binding = 1) uniform PointLight
{
	vec3 Position;
	vec4 Color;
	float Ambient;
	float Specular;
};

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
	vec4 Color;
};

layout(location = 0) out Vertex outVertex;
layout(location = 4) out flat int vEntityID;

void main()
{
	outVertex.Position = aPosition;
	outVertex.Normal = aNormal;
	outVertex.TexCoord = aTexCoord;
	outVertex.Color = aColor;
	vEntityID = aEntityID;

	gl_Position = cameraBuffer.ViewProjection * vec4(aPosition, 1.0);
}

// type fragment
#version 460 core

void main()
{

}