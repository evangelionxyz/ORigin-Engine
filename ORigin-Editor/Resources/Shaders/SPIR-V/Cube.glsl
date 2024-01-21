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
#version 450 
layout(location = 0) out vec4 oColor;
layout(location = 1) out vec4 oHDRColor;
layout(location = 2) out int oEntityID;

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
	vec4 texColor = In.Color;
	switch(int(v_TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], In.TexCoord); break;
		case  1: texColor *= texture(u_Textures[ 1], In.TexCoord); break;
		case  2: texColor *= texture(u_Textures[ 2], In.TexCoord); break;
		case  3: texColor *= texture(u_Textures[ 3], In.TexCoord); break;
		case  4: texColor *= texture(u_Textures[ 4], In.TexCoord); break;
		case  5: texColor *= texture(u_Textures[ 5], In.TexCoord); break;
		case  6: texColor *= texture(u_Textures[ 6], In.TexCoord); break;
		case  7: texColor *= texture(u_Textures[ 7], In.TexCoord); break;
		case  8: texColor *= texture(u_Textures[ 8], In.TexCoord); break;
		case  9: texColor *= texture(u_Textures[ 9], In.TexCoord); break;
		case 10: texColor *= texture(u_Textures[10], In.TexCoord); break;
		case 11: texColor *= texture(u_Textures[11], In.TexCoord); break;
		case 12: texColor *= texture(u_Textures[12], In.TexCoord); break;
		case 13: texColor *= texture(u_Textures[13], In.TexCoord); break;
		case 14: texColor *= texture(u_Textures[14], In.TexCoord); break;
		case 15: texColor *= texture(u_Textures[15], In.TexCoord); break;
		case 16: texColor *= texture(u_Textures[16], In.TexCoord); break;
		case 17: texColor *= texture(u_Textures[17], In.TexCoord); break;
		case 18: texColor *= texture(u_Textures[18], In.TexCoord); break;
		case 19: texColor *= texture(u_Textures[19], In.TexCoord); break;
		case 20: texColor *= texture(u_Textures[20], In.TexCoord); break;
		case 21: texColor *= texture(u_Textures[21], In.TexCoord); break;
		case 22: texColor *= texture(u_Textures[22], In.TexCoord); break;
		case 23: texColor *= texture(u_Textures[23], In.TexCoord); break;
		case 24: texColor *= texture(u_Textures[24], In.TexCoord); break;
		case 25: texColor *= texture(u_Textures[25], In.TexCoord); break;
		case 26: texColor *= texture(u_Textures[26], In.TexCoord); break;
		case 27: texColor *= texture(u_Textures[27], In.TexCoord); break;
		case 28: texColor *= texture(u_Textures[28], In.TexCoord); break;
		case 29: texColor *= texture(u_Textures[29], In.TexCoord); break;
		case 30: texColor *= texture(u_Textures[30], In.TexCoord); break;
		case 31: texColor *= texture(u_Textures[31], In.TexCoord); break;
	};

	oColor = texColor;
	oEntityID = v_EntityID;
}