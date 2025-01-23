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

void main()
{
    mat4 bone_transform = ubone_transforms[bone_ids[0]] * weights[0];
    bone_transform     += ubone_transforms[bone_ids[1]] * weights[1];
    bone_transform     += ubone_transforms[bone_ids[2]] * weights[2];
    bone_transform     += ubone_transforms[bone_ids[3]] * weights[3];

  gl_Position = CameraBuffer.view_projection * bone_transform * umodel_transform * vec4(position, 1.0);

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

void main()
{
  vec3 texcolor = texture(udiffuse_texture, vin.texcoord).rgb;
  frag_color = vec4(vin.color * texcolor, 1.0);
}
