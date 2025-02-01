// type vertex
#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texcoord;
} vout;

uniform mat4 transform;

void main()
{
    vout.position = vec4(transform * vec4(position, 1.0)).xyz;
    vout.normal = vec4(transform * vec4(normal, 0.0)).xyz;
    vout.texcoord = texcoord;
}

// type fragment
#version 450 core
void main()
{
}

// type geometry
#version 450 core

layout(std140, binding = 3) uniform ReflectPlane
{
    mat4 view_projection_cube[6];
};

layout(triangles, invocations = 6) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 position[];
layout(location = 1) in vec3 normal[];
layout(location = 2) in vec2 texcoord[];

layout(location = 0) out Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texcoord;
} vout;

void main()
{
    vec4 position_vp_temp[3];
    int out_of_bound[6] = int[6](0, 0, 0, 0, 0, 0);

    for (int i = 0; i < 3; ++i)
    {
        position_vp_temp[i] = view_projection_cube[gl_InvocationID] * vec4(position[i], 1.0);

        if (position_vp_temp[i].x > position_vp_temp[i].w)
            out_of_bound[0] = out_of_bound[0] + 1;
        if (position_vp_temp[i].x < -position_vp_temp[i].w)
            out_of_bound[1] = out_of_bound[1] + 1;
        if (position_vp_temp[i].y > position_vp_temp[i].w)
            out_of_bound[2] = out_of_bound[2] + 1;
        if (position_vp_temp[i].y < -position_vp_temp[i].w)
            out_of_bound[3] = out_of_bound[3] + 1;
        if (position_vp_temp[i].z > position_vp_temp[i].w)
            out_of_bound[4] = out_of_bound[4] + 1;
        if (position_vp_temp[i].z < -position_vp_temp[i].w)
            out_of_bound[5] = out_of_bound[5] + 1;
    }

    bool in_frustum = true;
    for (int i = 0; i < 6; ++i)
    {
        if (out_of_bound[i] == 3)
            in_frustum = false;
    }

    if (in_frustum)
    {
        for (int i = 0; i < 3; ++i)
        {
            vout.position = position[i];
            gl_Position = position_vp_temp[i];

            vout.normal = vec4(view_projection_cube[gl_InvocationID] * vec4(normal[i], 0.0)).xyz;

            vout.texcoord = texcoord[i];
            gl_Layer = gl_InvocationID;
            EmitVertex();
        }
        EndPrimitive();
    }
}