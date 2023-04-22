// type vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uViewProjection;
uniform vec3 uPos;
uniform vec3 uSize;

out vec3 vPos;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
	vPos = aPos + uPos;
	vNormal = aNormal;
	vTexCoord = aTexCoord;

	gl_Position = uViewProjection * (vec4(aPos + uPos, 1.0) * vec4(uSize, 1.0));
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;

uniform vec3 uLightPos;
uniform vec4 uLightColor;
uniform float uLightAmbient;

uniform vec3 uCameraPosition;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;

uniform sampler2D uTexture;

vec4 pointLight()
{
	vec3 lightVec = uLightPos - vPos;
	float dist = length(lightVec);
	float a = 3.0f;
	float b = 0.7f;
	float inten = 1.0f / (a * dist * dist * dist + b * dist + 1.0f);

	vec3 normal = normalize(vNormal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 1.5f;
	vec3 viewDirection = normalize(uCameraPosition - vPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specularAmount * specularLight;

	return texture(uTexture, vTexCoord) * uLightColor * (diffuse * inten + uLightAmbient + specular);
};

void main()
{
	oColor = pointLight();
}
