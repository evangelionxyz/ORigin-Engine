// type vertex
#version 450 core
layout (location = 0) in vec3 a_WorldPosition;
layout (location = 1) in vec3 a_LocalPosition;
layout (location = 2) in vec4 a_Color;
layout (location = 3) in float a_Thickness;
layout (location = 4) in float a_Fade;
layout (location = 5) in int a_EntityID;

layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec3 Position;

} CameraBuffer;

struct Vertex
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) out Vertex outVertex;
layout (location = 4) out flat int v_EntityID;

void main()
{
	outVertex.LocalPosition = a_LocalPosition;
	outVertex.Color = a_Color;
	outVertex.Thickness = a_Thickness;
	outVertex.Fade = a_Fade;
	v_EntityID = a_EntityID;

	gl_Position = CameraBuffer.ViewProjection * vec4(a_WorldPosition, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;
layout(location = 2) out int oEntityID;

struct Vertex
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) in Vertex inVertex;
layout (location = 4) in flat int v_EntityID;

void main()
{
	// Calculate distance and fill circle with white
  float distance = 1.0 - length(inVertex.LocalPosition);
  float circle = smoothstep(0.0, inVertex.Fade, distance);
  circle *= smoothstep(inVertex.Thickness + inVertex.Fade, inVertex.Thickness, distance);

	if (circle == 0.0) discard;

  // Set output color
  oColor = inVertex.Color;
	oColor.a *= circle;

	oEntityID = v_EntityID;
}