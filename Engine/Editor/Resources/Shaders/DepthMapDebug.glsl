// type vertex
#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out Vertex {
    vec2 texcoord;
} vout;

void main()
{
    vout.texcoord = texcoord;
    gl_Position = vec4(position, 1.0);
}

// type fragment
#version 450 core
layout (location = 0) out vec4 frag_color;
layout(location = 0) in Vertex {
    vec2 texcoord;
} vin;

uniform sampler2DArray udepth_map;
uniform int ucascade_index;

void main()
{
    float depth_value = texture(udepth_map, vec3(vin.texcoord, ucascade_index)).r;
    frag_color = vec4(vec3(depth_value), 1.0);
}