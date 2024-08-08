// type vertex
#version 450 core
layout (location = 0) in vec3 aTransformedPosition;
layout (location = 1) in vec3 aPosition;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aThickness;
layout (location = 4) in float aFade;
layout (location = 5) in int aEntityID;

layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec3 Position;

} CameraBuffer;

struct Vertex
{
	vec3 Position;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) out Vertex outVertex;
layout (location = 4) out flat int v_EntityID;

void main()
{
	outVertex.Position = aPosition;
	outVertex.Color = aColor;
	outVertex.Thickness = aThickness;
	outVertex.Fade = aFade;
	v_EntityID = aEntityID;
	gl_Position = CameraBuffer.ViewProjection * vec4(aTransformedPosition, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;
layout(location = 2) out int oEntityID;

struct Vertex
{
	vec3 Position;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) in Vertex inVertex;
layout (location = 4) in flat int v_EntityID;

void main()
{
  float distance = 1.0 - length(inVertex.Position);
  float circle = smoothstep(0.0, inVertex.Fade, distance);
  circle *= smoothstep(inVertex.Thickness + inVertex.Fade, inVertex.Thickness, distance);
  if (circle == 0.0)
    discard;

  oColor = inVertex.Color;
  oColor.a *= circle;
  oEntityID = v_EntityID;
}