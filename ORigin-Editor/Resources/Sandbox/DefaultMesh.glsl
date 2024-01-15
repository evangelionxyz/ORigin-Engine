// type vertex
#version 450 core
layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec3 Position;

} CameraBuffer;


layout(std140, binding = 1) uniform UBO
{
	mat4 Transform;
} Model;


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec2 texCoord;

void main()
{
	texCoord = aTexCoord;
	gl_Position = CameraBuffer.ViewProjection * Model.Transform * vec4(aPos, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;

layout(std140, binding = 0) uniform Material
{
	float Roughness;
	float Metalic;
} MaterialBuffer;

layout(binding = 0) uniform sampler2D texture_diffuse1;
layout(binding = 1) uniform sampler2D texture_specular1;

layout(location = 0) in vec2 texCoord;

void main()
{
	vec3 tex = texture(texture_diffuse1, texCoord).rgb;
	oColor = vec4(tex, 1.0);
}