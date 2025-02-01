// type vertex
#version 450 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 vcolor;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    vcolor = color;
    
}

// type fragment
#version 450
layout(location = 0) out vec4 frag_color;
layout(location = 0) in vec3 vcolor;

void main()
{
    frag_color = vec4(vcolor, 1.0);
}