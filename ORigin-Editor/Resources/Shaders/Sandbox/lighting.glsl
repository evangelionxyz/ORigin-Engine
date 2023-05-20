// type vertex
#version 450 core
uniform vec3 uLightPosition;
uniform mat4 uViewProjection;

void main()
{
	gl_Position = uViewProjection * vec4(uLightPosition, 1.0) * vec4(0.5f);
}

// type fragment
#version 450 core

void main()
{
}
