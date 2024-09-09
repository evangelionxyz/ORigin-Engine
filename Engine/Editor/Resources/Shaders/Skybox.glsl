// type vertex
#version 450 core
layout(location = 0) in vec3 aPos;

uniform mat4 uViewProjection;
uniform float scale = 1000.0f;

out vec3 vTexCoord;

void main()
{
	vTexCoord = aPos;
	vec4 Position = uViewProjection * vec4(aPos * scale, 1);
	gl_Position = Position.xyww;
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;
layout(location = 1) out int entColor;

in vec3 vTexCoord;
uniform float blurFactor = 0.0f;
uniform samplerCube uSkybox;

void main()
{
	vec3 color = vec3(0.0);
    
    // Sample the texture multiple times and average the colors
    for(int i = -2; i <= 2; i++)
    {
        for(int j = -2; j <= 2; j++)
        {
            color += texture(uSkybox, vTexCoord + vec3(float(i) * blurFactor, float(j) * blurFactor, 0.0)).rgb;
        }
    }
    
    // Divide the total color by the number of samples
    color /= 25.0;

    entColor = -1;
	oColor = vec4(color, 1.0);
}
