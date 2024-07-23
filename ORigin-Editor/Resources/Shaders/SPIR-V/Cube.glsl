// OpenGL-Shader

// type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;

layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec3 Position;

} CameraBuffer;

struct Vertex
{
	vec3 Position;
	vec4 Color;
	vec2 TexCoord;
};

layout (location = 0) out Vertex Out;
layout (location = 3) out flat float v_TexIndex;
layout (location = 4) out flat int v_EntityID;

void main()
{
	Out.Position = a_Position;
	Out.Color = a_Color;
	Out.TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_EntityID = a_EntityID;

	gl_Position = CameraBuffer.ViewProjection * vec4(a_Position, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

struct Vertex
{
	vec3 Position;
	vec4 Color;
	vec2 TexCoord;
};

layout(location = 0) in Vertex In;
layout(location = 3) in flat float v_TexIndex;
layout(location = 4) in flat int v_EntityID;

layout(binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	oColor = In.Color * texture(u_Textures[int(v_TexIndex)], In.TexCoord);
	oEntityID = v_EntityID;
}