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
} CameraBuffer;

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
    gl_Position = CameraBuffer.view_projection * umodel_transform * bone_transform * vec4(position, 1.0);
  }
  else
  {
    gl_Position = CameraBuffer.view_projection * umodel_transform * vec4(position, 1.0);
  }

  vout.position = position;
  vout.normals = normals;
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

uniform sampler2D udiffuse_texture;

// Existing hardcoded light and view positions
vec3 light_position = vec3(3.0, 0.0, 4.0);
vec3 view_position = vec3(0.0, -0.124, 1.0);
vec3 light_color = vec3(0.882, 0.6321, 1.0);

// Dummy hardcoded light source
vec3 dummy_light_position = vec3(-2.0, 2.0, 2.0);
vec3 dummy_light_color = vec3(0.4, 0.6, 0.9);

void main()
{
    // Existing light calculations
    vec3 ambient_color = 0.8 * light_color;
    vec3 norm = normalize(vin.normals);
    vec3 light_direction = normalize(light_position - vin.position);

    float diffuse = max(dot(norm, light_direction), 0.0);
    vec3 diffuse_color = diffuse * light_color;

    vec3 view_direction = normalize(view_position - vin.position);
    vec3 reflect_direction = reflect(-light_direction, norm);

    float specular = pow(max(dot(view_direction, reflect_direction), 0.0), 32);
    vec3 specular_color = specular * light_color;

    // Dummy light calculations
    vec3 dummy_light_direction = normalize(dummy_light_position - vin.position);
    float dummy_diffuse = max(dot(norm, dummy_light_direction), 0.0);
    vec3 dummy_diffuse_color = dummy_diffuse * dummy_light_color;

    vec3 dummy_reflect_direction = reflect(-dummy_light_direction, norm);
    float dummy_specular = pow(max(dot(view_direction, dummy_reflect_direction), 0.0), 16); // Lower specular power
    vec3 dummy_specular_color = dummy_specular * dummy_light_color;

    // Combine original and dummy lighting
    vec3 lighting = (ambient_color + diffuse_color + specular_color 
                     + dummy_diffuse_color + dummy_specular_color) * vin.color;

    // Texture color
    vec3 diffuse_texture_color = texture(udiffuse_texture, vin.texcoord).rgb;

    frag_color = vec4(diffuse_texture_color * lighting, 1.0);
}