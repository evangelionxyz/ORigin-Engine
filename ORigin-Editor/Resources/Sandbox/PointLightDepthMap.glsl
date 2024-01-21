// type vertex
#version 450 core
layout(location = 0) in vec3 position;


uniform mat4 modelTransform;

void main()
{
	gl_Position = modelTransform * vec4(position, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) in vec4 fragPosition;

uniform vec3 lightPosition;
uniform float farPlane;

void main()
{
	gl_FragDepth = length(fragPosition.xyz - lightPosition) / farPlane;
}

// type geometry
#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

layout(location = 0) out vec4 fragPosition;

void main()
{
	for(int face = 0; face < 6; face++)
	{
		gl_Layer = face;
		for(int i = 0; i < 3; i++)
		{
			fragPosition = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * fragPosition;
			EmitVertex();
		}

		EndPrimitive();
	}
}
