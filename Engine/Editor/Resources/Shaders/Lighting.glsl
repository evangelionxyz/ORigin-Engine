// type vertex
#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(location = 0) out Vertex
{
    vec3 position;
    vec4 light_space[3];
} vout;

uniform mat4 ulight_matrices[3];
uniform mat4 uview_projection;
uniform mat4 umodel_matrix;

void main()
{
    vec4 world_pos = umodel_matrix * vec4(position, 1.0);
    vout.position = world_pos.xyz;

    // transform fragment position into each cascade's light space
    for (int i = 0; i < 3; ++i)
    {
        vout.light_space[i] = ulight_matrices[i] * world_pos;
    }

    gl_Position = uview_projection * world_pos;
}

// type fragment
#version 450 core
layout(location = 0) out vec4 frag_color;

layout(location = 0) in Vertex
{
    vec3 position;
    vec4 light_space[3];
} vin;

uniform vec3 ulight_dir;
uniform vec4 ucascade_splits;
uniform int unum_cascades;
layout(binding = 3) uniform sampler2DArray ushadow_map; // shadow map array

float calculate_shadow(vec4 light_space, int cascade_index)
{
    vec3 projection_coords = light_space.xyz / light_space.w; // perspective division
    projection_coords = projection_coords * 0.5 + 0.5; // transform to [0, 1] range

    if (projection_coords.z > 1.0 || projection_coords.x < 0.0 || projection_coords.x > 1.0 || projection_coords.y < 0.0 || projection_coords.y > 1.0)
    {
      return 1.0; // fully lit
    }

    // sample the shadow map
    float shadow_depth = texture(ushadow_map, vec3(projection_coords.xy, cascade_index)).r;
    float current_depth = projection_coords.z;

    float bias = max(0.005 * (1.0 - dot(normalize(ulight_dir), normalize(vin.position))), 0.001);

    return current_depth - bias > shadow_depth ? 0.0 : 1.0;
}

void main()
{
    float shadow = 1.0;

    for (int i = 0; i < unum_cascades; ++i)
    {
        if (vin.position.z < ucascade_splits[i])
        {
            shadow = calculate_shadow(vin.light_space[i], i);
            break;
        }
    }

    vec3 base_color = vec3(0.8);
    vec3 light_color = vec3(1.0);
    vec3 lighting = shadow * light_color * base_color;
    frag_color = vec4(lighting, 1.0);
}