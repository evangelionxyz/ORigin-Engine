// type vertex
#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 viewProjection;

void main()
{
    gl_Position = viewProjection * vec4(aPos, 1.0);
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;


void main()
{
    oColor = vec4(1.0, 0.5, 0.0, 1.0);
}