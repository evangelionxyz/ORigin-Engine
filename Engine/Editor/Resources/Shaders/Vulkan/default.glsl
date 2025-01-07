// type vertex
#version 450 core

void main()
{
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}

// type fragment
#version 450

layout(location = 0) out vec4 oColor;

void main()
{
    oColor = vec4(1.0);
}