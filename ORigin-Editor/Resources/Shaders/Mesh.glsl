// type vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in float aTexIndex;
layout (location = 5) in int aEntityId;

uniform mat4 uViewProjection;
uniform mat4 uTransform;

layout (location = 0) out vec3 vPosition;
layout (location = 1) out vec3 vNormal;
layout (location = 2) out vec4 vColor;
layout (location = 3) out flat int vEntityID;

void main()
{
    vPosition = vec3(uTransform * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(uTransform))) * aNormals;
		vColor = aColor;
		vEntityID = aEntityId;

	  gl_Position = uViewProjection * vec4(vPosition, 1.0);
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;
layout (location = 1) out int oEntityID;

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec4 vColor;
layout (location = 3) in flat int vEntityID;

uniform vec3 uColor;
uniform vec3 uLightingPosition;

float lighting(vec3 normal)
{
    vec3 lightDirection = normalize(uLightingPosition);
    vec3 reflectDirection = reflect(lightDirection, normal);
    float diffuseContrib = max(dot(lightDirection, normal), 0.0);
    return diffuseContrib;
}

void main()
{
    oColor = vec4(lighting(vNormal) * uColor, 1.0);
		oEntityID = vEntityID;
}