// type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in int a_EntityID;

layout (std140, binding = 0) uniform UBO
{
	mat4 ViewProjection;
	mat4 LightSpaceMatrix;
	vec3 CameraPosition;

} uboData;

struct VertexOutput
{
	vec4 Color;
};

layout (location = 0) out VertexOutput Output;
layout (location = 1) out flat int v_EntityID;

void main()
{
	Output.Color = a_Color;
	v_EntityID = a_EntityID;

	gl_Position = uboData.ViewProjection * vec4(a_Position, 1.0);
}

// type fragment
#version 450 core
layout (location = 0) out vec4 Color;
layout (location = 1) out int entityID;

struct VertexOutput
{
	vec4 Color;
};

layout (location = 0) in VertexOutput Input;
layout (location = 1) in flat int v_EntityID;

void main()
{
	Color = Input.Color;
	entityID = v_EntityID;
}