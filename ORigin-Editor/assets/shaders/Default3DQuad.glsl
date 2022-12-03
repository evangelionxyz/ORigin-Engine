//type vertex
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

out vec3 v_Pos;
out vec4 v_Color;

uniform mat4 u_ViewProjection;

void main()
{
	v_Pos = aPos;
	v_Color = aColor;
	gl_Position = u_ViewProjection * vec4(aPos, 1.0f);
}

//type fragment
#version 460 core

in vec3 v_Pos;
out vec4 color;

uniform sampler2D u_Textures;

void main()
{
	color = vec4(v_Pos, 1.0f);
}