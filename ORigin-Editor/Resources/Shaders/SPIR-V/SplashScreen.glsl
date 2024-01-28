// type vertex
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 vTexCoord;
void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	vTexCoord = aTexCoord;
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;

layout(location = 0) in vec2 vTexCoord;

layout(binding = 0) uniform sampler2D uTexture;

void main()
{
	oColor = texture(uTexture, vTexCoord);
}