// type vertex
#version 330 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 texCoords;

void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
    texCoords = inTexCoords;
}

// type fragment
#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float gamma;

void main()
{
    vec3 fragment = texture(screenTexture, texCoords).rgb;

    float exposure = 0.1f;
    vec3 toneMapped = vec3(1.0f) - exp(-fragment * exposure);

    FragColor.rgb = pow(toneMapped, vec3(1.0f / gamma));
}