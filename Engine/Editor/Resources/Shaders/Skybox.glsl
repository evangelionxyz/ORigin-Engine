// type vertex
#version 450 core
layout(location = 0) in vec3 position;

layout(std140, binding = 0) uniform Camera
{
    mat4 view_projection;
    vec3 position;
} camera_buffer;

layout(location = 0) out Vertex
{
  vec3 texcoord;
} vout;

void main()
{
  mat4 view_no_translation = mat4(mat3(camera_buffer.view_projection));

	vec4 pos = view_no_translation * vec4(position, 1.0);
	gl_Position = vec4(pos.xy, pos.z, pos.z);
	vout.texcoord = position;
}

// type fragment
#version 450 core
layout(location = 0) out vec4 frag_color;

layout(location = 0) in Vertex
{
  vec3 texcoord;
} vin;

uniform float ublur_factor;
uniform samplerCube uskybox_cube;

void main()
{
  vec3 color = vec3(0.0);
  for (int i = -2; i < 2; ++i)
  {
    for (int j = -2; j < 2; ++j)
    {
      color += texture(uskybox_cube, 
        vin.texcoord + vec3(float(i) * ublur_factor, float(j) * ublur_factor, 0.0)).rgb;
    }
  }
  color /= 25.0;
  frag_color = vec4(color, 1.0);
}
