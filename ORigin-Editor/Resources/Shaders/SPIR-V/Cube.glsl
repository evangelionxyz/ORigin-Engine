// OpenGL-Shader

// type vertex
#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexIndex;
layout(location = 4) in int aEntityID;

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
	Out.Position = aPosition;
	Out.Color = aColor;
	Out.TexCoord = aTexCoord;
	v_TexIndex = aTexIndex;
	v_EntityID = aEntityID;

	gl_Position = CameraBuffer.ViewProjection * vec4(aPosition, 1.0);
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