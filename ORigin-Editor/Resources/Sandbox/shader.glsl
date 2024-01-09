// type vertex
#version 450 core
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec3 aColor;

layout (std140, binding = 1) uniform myBuffer
{
	mat4 projection;
	mat4 view;
	vec3 color;
};

void main()
{
	gl_Position = projection * view * vec4(aPosition, 0.0, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;

layout (std140, binding = 1) uniform myBuffer
{
	mat4 projection;
	mat4 view;
	vec3 color;
};

void main()
{
	oColor = vec4(color, 1.0);
}