// type vertex
#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 uViewProjection;
uniform vec3 uPos;
uniform vec3 uSize;

void main()
{
	gl_Position = uViewProjection * vec4((aPos + uPos) * uSize, 1.0);
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;

uniform vec4 uLightColor;

void main()
{
	oColor = uLightColor;
}
