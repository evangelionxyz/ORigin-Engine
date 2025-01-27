// type vertex
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoord;
layout (location = 4) in ivec4 bone_ids;
layout (location = 5) in vec4 weights;

const int MAX_BONES = 200;
const int NUM_CASCADES = 3;

layout (location = 0) out Vertex
{
  vec3 position;
  vec3 normals;
  vec3 color;
  vec2 texcoord;
  flat ivec4 bone_ids;
  vec4 weights;
  vec4 light_space[NUM_CASCADES];
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
            vout.light_space[i] = ulight_matrices[i] * umodel_transform * bone_transform * vec4(position, 1.0);
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
            vout.light_space[i] = ulight_matrices[i] * umodel_transform * vec4(position, 1.0);
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

const int NUM_CASCADES = 3;

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
    vec4 light_space[NUM_CASCADES];
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

layout(binding = 6) uniform sampler2D udiffuse_texture;
layout(binding = 7) uniform sampler2D uspecular_texture;
layout(binding = 8) uniform sampler2D uroughness_texture;
layout(binding = 9) uniform sampler2DArray ushadow_map;

uniform int uspot_light_count;
uniform int umaterial_index;
uniform vec4 ucascade_splits;
uniform int unum_cascades;

Material material = materials[umaterial_index];

/// ====================================
///         SHADOW CALCULATION
/// ====================================
float calculate_shadow(vec4 light_space, int cascade_index)
{
    if (light_space.w == 0.0) {
      return 1.0; // fully lit, as this is invalid
    }

    vec3 projection_coords = light_space.xyz / light_space.w; // perspective division
    projection_coords = projection_coords * 0.5 + 0.5; // transform to [0, 1] range

    if (projection_coords.z > 1.0 || projection_coords.x < 0.0 || projection_coords.x > 1.0 || projection_coords.y < 0.0 || projection_coords.y > 1.0)
    {
      return 1.0; // fully lit
    }

    // sample the shadow map
    float shadow_depth = texture(ushadow_map, vec3(projection_coords.xy, cascade_index)).r;
    float current_depth = projection_coords.z;

    float bias = max(0.005 * (1.0 - dot(normalize(dir_light_buffer.direction.xyz), normalize(vin.position))), 0.001);

    return current_depth - bias > shadow_depth ? 0.0 : 1.0;
}

/// ====================================
///        SPOT LIGHT CALCULATION
/// ====================================
vec3 calculate_spot_light(SpotLight light, vec3 view_dir, vec3 frag_pos, vec3 normal)
{
    vec3 normalized_view_dir = normalize(view_dir);
    vec3 light_dir = normalize(light.direction.xyz);
    
    // direction to fragment
    vec3 to_frag = normalize(frag_pos - light.position.xyz);

    // angel between light direction and fragment direction
    float theta = dot(-light_dir, to_frag);

    // check if within spotlight cone
    float epsilon = light.cut_off.x - light.cut_off.y;
    float intensity = clamp((theta - light.cut_off.y) / epsilon, 0.0, 1.0);

    vec3 ambient_color = 0.1 * light.color.rgb;

    float diffuse = max(dot(normal, -light_dir), 0.0);
    vec3 diffuse_color = diffuse * light.color.rgb;

    // specular phong
    vec3 reflect_dir = reflect(light_dir, normal);
    float specular = pow(max(dot(normalized_view_dir, reflect_dir), 0.0), 32.0);
    vec3 specular_color = specular * light.color.rgb;

    // combine
    return (ambient_color + intensity * (diffuse_color + specular_color)) * light.color.a;
}

vec3 calculate_all_spotlights(vec3 view_dir, vec3 frag_pos, vec3 normal)
{
    vec3 result = vec3(0.0);

    for (int i = 0; i < uspot_light_count; ++i)
    {
        result += calculate_spot_light(spot_lights[i], view_dir, frag_pos, normal);
    }

    return result;
}

/// ====================================
///   DIRECTIONAL LIGHT CALCULATION
/// ====================================

vec3 calculate_directional_light(vec3 normal, vec3 view_dir, vec2 texcoord)
{
  vec3 light_direction = normalize(-dir_light_buffer.direction.xyz); // light direction (inverted for incoming light)
  vec3 normalized_normal = normalize(normal);
  vec3 normalized_view_dir = normalize(view_dir);

  // ambient light contribution
  float ambient_strength = 0.1;
  vec3 ambient_color = ambient_strength * dir_light_buffer.color.rgb;

  // diffuse light contribution
  float diffuse = max(dot(normalized_normal, light_direction), 0.0);
  vec3 diffuse_color = diffuse * dir_light_buffer.color.rgb;

  // specular light contribution (phong)
  float specular_strength = 0.5;
  vec3 reflect_dir = reflect(-light_direction, normalized_normal); // reflected light vector
  float specular = pow(max(dot(normalized_view_dir, reflect_dir), 0.0), 32.0); // shininess = 16
  vec3 specular_color = specular_strength * specular * dir_light_buffer.color.rgb;

  // combine the contributions
  vec3 base_color = texture(udiffuse_texture, texcoord).rgb;
  return (ambient_color + diffuse_color + specular_color) * base_color * material.base_color.rgb;
}

void main()
{
    vec3 normal = vin.normals;
    vec3 view_dir = camera_buffer.position - vin.position;
    vec3 frag_pos = normalize(vin.position);

    // shadow
    float shadow = 1.0;
    for (int i = 0; i < unum_cascades; ++i)
    {
        if (vin.light_space[i].z < ucascade_splits[i])
        {
            shadow = calculate_shadow(vin.light_space[i], i);
            break;
        }
    }

    // calculate lighting
    vec3 lighting = vec3(0.0);
    lighting += calculate_all_spotlights(view_dir, frag_pos, normal);
    lighting += calculate_directional_light(normal, view_dir, vin.texcoord * material.tiling_factor.xy);

    frag_color = vec4(shadow * lighting, 1.0);
}