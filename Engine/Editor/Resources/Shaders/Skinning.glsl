// type vertex
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoord;
layout (location = 4) in ivec4 bone_ids;
layout (location = 5) in vec4 weights;

const int MAX_BONES = 200;
const int NUM_CASCADES = 4;

layout (location = 0) out Vertex
{
  vec3 position;
  vec3 normals;
  vec3 color;
  vec2 texcoord;
  flat ivec4 bone_ids;
  vec4 weights;
  vec4 light_space_position[NUM_CASCADES];
} vout;

layout(std140, binding = 0) uniform Camera
{
    mat4 view_projection;
    vec3 position;
} camera_buffer;

uniform mat4 ubone_transforms[MAX_BONES];
uniform mat4 ulight_matrices[NUM_CASCADES];
uniform mat4 umodel_transform;
uniform bool uhas_animation;

void main()
{
    if (uhas_animation)
    {
        mat4 bone_transform = ubone_transforms[bone_ids[0]] * weights[0];
        bone_transform     += ubone_transforms[bone_ids[1]] * weights[1];
        bone_transform     += ubone_transforms[bone_ids[2]] * weights[2];
        bone_transform     += ubone_transforms[bone_ids[3]] * weights[3];

        for (int i = 0; i < NUM_CASCADES; ++i)
        {
            vout.light_space_position[i] = ulight_matrices[i] * umodel_transform * bone_transform * vec4(position, 1.0);
        }

        gl_Position = camera_buffer.view_projection * umodel_transform * bone_transform * vec4(position, 1.0);

        // bone transformation for the normal (3x3 matrix, ignores translation)
        mat3 bone_normal_transform = mat3(ubone_transforms[bone_ids[0]]) * weights[0];
        bone_normal_transform     += mat3(ubone_transforms[bone_ids[1]]) * weights[1];
        bone_normal_transform     += mat3(ubone_transforms[bone_ids[2]]) * weights[2];
        bone_normal_transform     += mat3(ubone_transforms[bone_ids[3]]) * weights[3];

        // transform the normal using the combined bone normal transform
        mat3 normal_matrix = mat3(transpose(inverse(mat3(umodel_transform))));
        vout.normals = normalize(normal_matrix * bone_normal_transform * normals);
    }
    else
    {
        for (int i = 0; i < NUM_CASCADES; ++i)
        {
            vout.light_space_position[i] = ulight_matrices[i] * umodel_transform * vec4(position, 1.0);
        }

        gl_Position = camera_buffer.view_projection * umodel_transform * vec4(position, 1.0);
        mat3 normal_matrix = mat3(transpose(inverse(mat3(umodel_transform))));
        vout.normals = normalize(normal_matrix * normals);
    }

    vout.position = position;
    vout.color = color;
    vout.texcoord = texcoord;
    vout.bone_ids = bone_ids;
    vout.weights = weights;
}

// type fragment
#version 450 core

const int NUM_CASCADES = 4;

struct Material
{
    vec4 base_color;
    vec4 tiling_factor;
};

struct SpotLight
{
    vec4 position;
    vec4 direction;
    vec4 color;
    vec2 cut_off;
};

layout (location = 0) out vec4 frag_color;

layout (location = 0) in Vertex
{
    vec3 position;
    vec3 normals;
    vec3 color;
    vec2 texcoord;
    flat ivec4 bone_ids;
    vec4 weights;
    vec4 light_space_position[NUM_CASCADES];
} vin;

/// ====================================
//          STORAGE BUFFER
/// ====================================
layout(std140, binding = 0) buffer MaterialBuffer
{
    Material materials[];
};

layout(std140, binding = 1) buffer SpotLightBuffer
{
    SpotLight spot_lights[];
};

layout(std140, binding = 0) uniform Camera
{
    mat4 view_projection;
    vec3 position;
} camera_buffer;

layout(std140, binding = 1) uniform Lighting
{
    int directional_light_count;
    int spotlight_count;
    int point_light_count;
    int area_light_count;
} LightingBuffer;

layout(std140, binding = 2) uniform DirectionalLight
{
    vec4 color;     // Light light color
    vec4 direction; // light direction (should be normalized)
} dir_light_buffer;


layout(std140, binding = 3) uniform PointLight
{
    vec3 position;
    vec3 color;
} point_light_buffer;

layout(std140, binding = 4) uniform AreaLight
{
    vec3 position;
    vec3 color;
} area_light_buffer;

layout(binding = 0) uniform sampler2D udiffuse_texture;
layout(binding = 1) uniform sampler2D uspecular_texture;
layout(binding = 2) uniform sampler2D uroughness_texture;
layout(binding = 3) uniform sampler2DArrayShadow ushadow_map;

uniform int uspot_light_count;
uniform int umaterial_index;
uniform vec4 ucascade_splits;
uniform float ucascade_plane_distances[NUM_CASCADES];
uniform float ushadow_bias[NUM_CASCADES];

Material material = materials[umaterial_index];

/// ====================================
///         SHADOW CALCULATION
/// ====================================
const int pcf_size = 3;
const float total_samples = (pcf_size * 2.0 + 1.0) * (pcf_size * 2.0 + 1.0);
int get_cascade_level(float frag_depth)
{
    for (int i = 0; i < NUM_CASCADES; ++i)
    {
        if (frag_depth < ucascade_plane_distances[i])
            return i;
    }
    return 3;
}
float calculate_shadow(vec3 frag_pos, vec3 normal, int cascade_index) {
    
    vec4 frag_pos_light_space = vin.light_space_position[cascade_index];
    vec3 projection_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    
    projection_coords = projection_coords * 0.5 + 0.5;
    
    if(projection_coords.x > 1.0 || projection_coords.x < 0.0 || 
       projection_coords.y > 1.0 || projection_coords.y < 0.0 ||
       projection_coords.z > 1.0 || projection_coords.z < 0.0) {
        return 0.0;
    }

    float bias = max(ushadow_bias[cascade_index] * 
        (1.0 - dot(normal, normalize(dir_light_buffer.direction.xyz))), 
        ushadow_bias[cascade_index] * 0.005);
    
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(ushadow_map, 0).xy;
    
    float current_depth = projection_coords.z - bias;
    
    const int hw_pcf_size = 2;
    float total_samples = float((hw_pcf_size * 2 + 1) * (hw_pcf_size * 2 + 1));
    
    for(int x = -hw_pcf_size; x <= hw_pcf_size; ++x) {
        for(int y = -hw_pcf_size; y <= hw_pcf_size; ++y) {
            shadow += texture(ushadow_map, 
                vec4(projection_coords.xy + vec2(x, y) * texel_size,
                     float(cascade_index),
                     current_depth));
        }
    }
    
    float fade_start = ucascade_plane_distances[cascade_index] 
                      - (ucascade_plane_distances[cascade_index] * 0.1);
    float fade_end = ucascade_plane_distances[cascade_index];
    float fade_length = fade_end - fade_start;
    float fade = 1.0 - clamp((projection_coords.z - fade_start) / fade_length, 0.0, 1.0);
    return (shadow / total_samples) * fade;
}

/// ====================================
///   DIRECTIONAL LIGHT CALCULATION
/// ====================================
vec3 calculate_directional_light(vec3 normal, vec3 view_dir, vec2 texcoord, float shadow)
{
  vec3 light_direction = normalize(-dir_light_buffer.direction.xyz);
  vec3 normalized_normal = normalize(normal);
  vec3 normalized_view_dir = normalize(view_dir);
  float ambient_strength = 0.1;
  vec3 ambient_color = ambient_strength * dir_light_buffer.color.rgb;
  float diffuse = max(dot(normalized_normal, light_direction), 0.0);
  vec3 diffuse_color = diffuse * dir_light_buffer.color.rgb;
  float specular_strength = 0.5;
  vec3 reflect_dir = reflect(-light_direction, normalized_normal);
  float specular = pow(max(dot(normalized_view_dir, reflect_dir), 0.0), 32.0); 
  vec3 specular_color = specular_strength * specular * dir_light_buffer.color.rgb;
  vec3 base_color = texture(udiffuse_texture, texcoord).rgb;
  return (ambient_color + (1.0 - shadow) * (diffuse_color + specular_color)) * base_color * material.base_color.rgb;
  //return (ambient_color + diffuse_color + specular_color) * base_color * material.base_color.rgb;
}

void main()
{
    vec3 normal = normalize(vin.normals);
    vec3 view_dir = normalize(camera_buffer.position - vin.position);

    // shadow
    float view_distance = length(camera_buffer.position - vin.position);
    int cascade_index = get_cascade_level(view_distance);
    float shadow = calculate_shadow(vin.position, normal, cascade_index);

    // calculate lighting
    vec3 lighting = vec3(0.0);
    lighting += calculate_directional_light(normal, view_dir, vin.texcoord * material.tiling_factor.xy, shadow);

    if(cascade_index == 0)
        lighting *= vec3(1.0, 0.85, 0.85);
    else if(cascade_index == 1)
        lighting *= vec3(0.85, 1.0, 0.85);
    else if(cascade_index == 2)
        lighting *= vec3(0.85, 0.85, 1.0);
    else
        lighting *= vec3(0.85, 0.85, 1.0);


    frag_color = vec4(lighting, 1.0);
    //frag_color = vec4(vec3(shadow), 1.0);
}