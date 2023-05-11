// type vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
};

out Vertex vOut;

uniform mat4 uViewProjection;

void main()
{
	vOut.Position = aPos;
	vOut.Normal = aNormal;
	vOut.TexCoord = aTexCoord;

	gl_Position = uViewProjection * vec4(aPos, 1.0);
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
};

in Vertex vIn;

void main()
{
oColor = vec4(1.0, 0.8, 0.9, 1.0);
}