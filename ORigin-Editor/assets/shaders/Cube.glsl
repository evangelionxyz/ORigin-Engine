// OpenGL-Shader

// type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
} CameraBuffer;

struct VertexOutput
{
	vec3 Normal;
	vec4 Color;
};

layout (location = 0) out VertexOutput Output;
layout (location = 2) out flat int v_EntityID;

void main()
{
	Output.Normal = a_Normal;
	Output.Color = a_Color;
	v_EntityID = a_EntityID;

	gl_Position = CameraBuffer.ViewProjection * vec4(a_Position, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 color;
layout(location = 1) out int entColor;

struct VertexOutput
{
	vec3 Normal;
	vec4 Color;
};

layout (location = 0) in VertexOutput Input;
layout (location = 2) in flat int v_EntityID;

void main()
{
	color = Input.Color;
	entColor = v_EntityID;
}