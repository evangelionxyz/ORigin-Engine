//type vertex
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aTexCoords;

out vec3 vPos;
out vec3 v_TexCoords;
float size = 1000.0f;

uniform mat4 u_ViewProjection;

void main()
{
	v_TexCoords = aTexCoords;

	vec4 Position = u_ViewProjection * vec4(aPos * size, 1.0f);
	gl_Position = Position.xyww;
}

//type fragment
#version 460 core
in vec3 v_TexCoords;
out vec4 color;

uniform samplerCube skybox;

void main()
{
	color = texture(skybox, v_TexCoords);
}
