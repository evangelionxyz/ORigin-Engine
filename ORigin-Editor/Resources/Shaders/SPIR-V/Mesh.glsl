// type vertex
#version 450 core
layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec3 Position;

} CameraBuffer;

layout(std140, binding = 1) uniform ModelBuffer
{
	mat4 ModelTransform;
	int EntityID;
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec2 texCoord;

void main()
{
	texCoord = aTexCoord;
	gl_Position = CameraBuffer.ViewProjection * ModelTransform * vec4(aPos, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

layout(std140, binding = 1) uniform ModelBuffer
{
	mat4 ModelTransform;
	int EntityID;
};

layout(binding = 0) uniform sampler2D u_Texture;

layout(location = 0) in vec2 texCoord;

void main()
{
	vec3 tex = texture(u_Texture, texCoord).rgb;

	oColor = vec4(tex, 1.0);
	oEntityID = EntityID;
}