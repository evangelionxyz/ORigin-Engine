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
uniform vec3 uPos;
uniform vec3 uSize;

void main()
{
	vOut.Position = aPos + uPos;
	vOut.Normal = aNormal;
	vOut.TexCoord = aTexCoord;

	gl_Position = uViewProjection * (vec4(aPos + uPos, 1.0) * vec4(uSize, 1.0));
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;

uniform vec3 uLightPos;
uniform vec4 uLightColor;
uniform float uLightAmbient;
uniform vec3 uCameraPosition;

uniform sampler2D uTexture;

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
};

out Vertex vIn;

vec4 pointLight()
{
	vec3 lightVec = uLightPos - vIn.Position;
	float dist = length(lightVec);
	float a = 3.0f;
	float b = 0.7f;
	float inten = 1.0f / (a * dist * dist * dist + b * dist + 1.0f);

	vec3 normal = normalize(vIn.Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 1.5f;
	vec3 viewDirection = normalize(uCameraPosition - vIn.Position);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specularAmount * specularLight;

	return texture(uTexture, vIn.TexCoord) * uLightColor * (diffuse * inten + uLightAmbient + specular);
};

void main()
{
	oColor = pointLight();
}
