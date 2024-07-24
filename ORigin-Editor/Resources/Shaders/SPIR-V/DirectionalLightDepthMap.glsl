// type vertex
#version 460 core
layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform DepthBuffer
{
	mat4 ViewProjection;
};

void main()
{
	gl_Position = ViewProjection * vec4(aPos, 1.0);
}

// type fragment
#version 460 core

void main()
{
}
