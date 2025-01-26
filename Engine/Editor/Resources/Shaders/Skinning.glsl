// type vertex
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoord;
layout (location = 4) in ivec4 bone_ids;
layout (location = 5) in vec4 weights;

const int MAX_BONES = 200;

layout (location = 0) out Vertex
{
  vec3 position;
  vec3 normals;
  vec3 color;
  vec2 texcoord;
  flat ivec4 bone_ids;
  vec4 weights;
} vout;

layout(std140, binding = 0) uniform Camera
{
    mat4 view_projection;
    vec3 position;
} camera_buffer;

uniform mat4 ubone_transforms[MAX_BONES];
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
layout (location = 0) out vec4 frag_color;

layout (location = 0) in Vertex
{
    vec3 position;
    vec3 normals;
    vec3 color;
    vec2 texcoord;
    flat ivec4 bone_ids;
    vec4 weights;
} vin;

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
    vec3 color;     // Light light color
    vec3 direction; // light direction (should be normalized)
} dir_light_buffer;

layout(std140, binding = 3) uniform SpotLight
{
    vec3 position;
    vec3 color;
} spot_light_buffer;

layout(std140, binding = 4) uniform PointLight
{
    vec3 position;
    vec3 color;
} point_light_buffer;

layout(std140, binding = 5) uniform AreaLight
{
    vec3 position;
    vec3 color;
} area_light_buffer;

struct Material
{
    vec4 base_color;
    vec4 tiling_factor;
};

layout(std140, binding = 0) buffer MaterialBuffer
{
    Material materials[];
};

layout(binding = 7) uniform sampler2D udiffuse_texture;
layout(binding = 8) uniform sampler2D uspecular_texture;
layout(binding = 9) uniform sampler2D uroughness_texture;
uniform int umaterial_index;

Material material = materials[umaterial_index];

vec3 calculate_directional_light(vec3 normal, vec3 view_dir, vec2 texcoord)
{
  vec3 light_direction = normalize(-dir_light_buffer.direction); // light direction (inverted for incoming light)
  vec3 normalized_normal = normalize(normal);
  vec3 normalized_view_dir = normalize(view_dir);

  // ambient light contribution
  float ambient_strength = 0.1;
  vec3 ambient_color = ambient_strength * dir_light_buffer.color;

  // diffuse light contribution
  float diffuse = max(dot(normalized_normal, light_direction), 0.0);
  vec3 diffuse_color = diffuse * dir_light_buffer.color;

  // specular light contribution (phong)
  float specular_strength = 0.5;
  vec3 reflect_dir = reflect(-light_direction, normalized_normal); // reflected light vector
  float specular = pow(max(dot(normalized_view_dir, reflect_dir), 0.0), 32.0); // shininess = 16
  vec3 specular_color = specular_strength * specular * dir_light_buffer.color;

  // combine the contributions
  vec3 base_color = texture(udiffuse_texture, texcoord).rgb;
  return (ambient_color + diffuse_color + specular_color) * base_color * material.base_color.rgb;
}

void main()
{
    vec3 normal = vin.normals;
    vec3 view_dir = camera_buffer.position - vin.position;

    vec3 lighting = vec3(0.0);
    lighting += calculate_directional_light(normal, view_dir, vin.texcoord * material.tiling_factor.xy);

    frag_color = vec4(lighting, 1.0);
}