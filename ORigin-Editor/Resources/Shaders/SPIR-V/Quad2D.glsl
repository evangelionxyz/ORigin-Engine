// type vertex
#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec2 aTilingFactor;
layout(location = 4) in float aTexIndex;

layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
} CameraBuffer;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	vec2 TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 3) out flat float v_TexIndex;
layout (location = 4) out flat int v_EntityID;

void main()
{
	Output.Color = aColor;
	Output.TexCoord = aTexCoord;
	Output.TilingFactor = aTilingFactor;
	v_TexIndex = aTexIndex;

	gl_Position = CameraBuffer.ViewProjection * vec4(aPosition, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	vec2 TilingFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 3) in flat float v_TexIndex;
layout (location = 4) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	vec4 texColor = Input.Color;
	texColor *= texture(u_Textures[int(v_TexIndex)], Input.TexCoord * Input.TilingFactor);
	oColor = texColor;
}