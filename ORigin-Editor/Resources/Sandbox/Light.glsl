// type vertex
#version 450 core
layout(location = 0) in vec3 position;

uniform mat4 viewProjection;
uniform mat4 modelTransform;

void main()
{
	gl_Position = viewProjection * modelTransform * vec4(position, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;

uniform vec3 color;

void main()
{
	oColor = vec4(color, 1.0);
}