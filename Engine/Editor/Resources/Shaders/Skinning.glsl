// type vertex
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec4 bone_ids;
layout (location = 4) in vec4 weights;

layout (location = 0) out Vertex
{
  vec3 position;
  vec2 texcoord;
  vec3 normal;
  flat ivec4 bone_ids;
  vec4 weights;
} vout;

layout(std140, binding = 0) uniform Camera
{
    mat4 view_projection;
    vec3 position;
} CameraBuffer;

uniform mat4 umodel;

void main()
{
  gl_Position = CameraBuffer.view_projection * umodel * vec4(position, 1.0);
  vout.position = position;
  vout.texcoord = texcoord;
  vout.normal = normal;
  vout.bone_ids = bone_ids;
  vout.weights = weights;
}

// type fragment
#version 450 core
layout (location = 0) out vec4 frag_color;

layout (location = 0) in Vertex
{
  vec3 position;
  vec2 texcoord;
  vec3 normal;
  flat ivec4 bone_ids;
  vec4 weights;
} vin;

#if VULKAN
layout (set=0, binding = 0) uniform int udisplay_bone_index;
#else
uniform int udisplay_bone_index;
#endif

void main()
{

  bool found = false;

  for (int i = 0; i < 4; ++i)
  {
    if (vin.bone_ids[i] == udisplay_bone_index)
    {
      if (vin.weights[i] >= 0.7)
        frag_color = vec4(1.0f, 0.0, 0.0, 1.0) * vin.weights[i];
      else if (vin.weights[i] >= 0.4 && vin.weights[i] <= 0.6)
        frag_color = vec4(0.0, 1.0, 0.0, 1.0) * vin.weights[i];
      else if (vin.weights[i] >= 0.1)
        frag_color = vec4(1.0, 1.0, 0.0, 1.0) * vin.weights[i];

      found = true;
      break;
    }
  }


  if (!found)
    frag_color = vec4(0.0, 0.0, 1.0, 1.0);
}
