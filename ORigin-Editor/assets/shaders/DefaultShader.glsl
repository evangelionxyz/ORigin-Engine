//type vertex
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec4 aColor;
layout(location = 3) in float aTexIndex;

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
in vec4 v_Color;

out vec4 color;

void main()
{
	else color = v_Color;
}