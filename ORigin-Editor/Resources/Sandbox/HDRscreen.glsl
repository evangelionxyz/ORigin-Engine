// type vertex
#version 450 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 fragTexCoord;

void main()
{
	gl_Position = vec4(position, 0.0, 1.0);
	fragTexCoord = texCoord;
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;

layout(location = 0) in vec2 fragTexCoord;

uniform sampler2D screenTexture;
uniform float Exposure;
uniform float Gamma;

void main()
{
  vec3 hdrColor = texture(screenTexture, fragTexCoord).rgb;
  vec3 finalColor = vec3(1.0) - exp(-hdrColor * Exposure);
	finalColor = pow(finalColor, vec3(1.0 / Gamma));
	oColor = vec4(finalColor, 1.0);
}