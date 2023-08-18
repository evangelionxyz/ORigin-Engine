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
out Vertex vertex;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
	vertex.Position = vec3(uModel * vec4(aPos, 1.0));
    vertex.Normal = mat3(transpose(inverse(uModel))) * aNormal;
	vertex.TexCoord = aTexCoord;

	gl_Position = uProjection * uView * vec4(vertex.Position, 1.0);
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
};
in Vertex vertex;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    float Shininess;
};
uniform Material material;

uniform vec3 uColor;
uniform int  uEntityID;
uniform bool uHasTexture;

/*
uniform sampler2D uShadowMap;

struct DirectionalLight
{
    vec3 Direction;
    vec3 Color;
    float Ambient;
    float Diffuse;
    float Specular;
};
uniform DirectionalLight light;


float shadowCalculation(vec4 lightSpacePosition)
{
    vec3 projectionCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projectionCoords = projectionCoords * 0.5 + 0.5;

    float closestDepth = texture(uShadowMap, projectionCoords.xy).r;
    float currentDepth = projectionCoords.z;

    vec3 normal = normalize(vertex.Normal);
    vec3 lightDirection = normalize(light.Direction - vertex.Position);
    float bias = max(0.005 * (1.0 - dot(normal, lightDirection)), 0.005);

    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMap, projectionCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }    
    }

    shadow /= 9.0;
    if(projectionCoords.z > 1.0) shadow = 0.0;

    return shadow;
}

vec3 lightCalculation(vec3 diffuseTexture, vec3 specularTexture)
{
    // Ambient
    vec3 ambient = light.Ambient * light.Color * diffuseTexture;

    // Diffuse
    vec3 norm = normalize(vertex.Normal);
    vec3 lightDirection = normalize(light.Direction);
    float diffuseContrib = max(dot(lightDirection, norm), 0.0);
    vec3 diffuse = diffuseContrib * light.Diffuse * light.Color * diffuseTexture;

    // Specular
    vec3 viewDirection = normalize(uCameraPosition - vertex.Position);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float specularContrib = pow(max(dot(viewDirection, reflectDirection), 0.0), material.Shininess);
    vec3 specular = light.Specular * specularContrib * light.Color * specularTexture;

    float shadow = shadowCalculation(vertex.LightSpacePosition);

    return ambient + (1.0 - shadow) * (diffuse + specular);
}
*/

void main()
{
    /*
    vec3 diffuseTex = vec3(1.0);
    vec3 specularTex = vec3(1.0);

    diffuseTex = uHasTexture ? texture(material.texture_diffuse1, vertex.TexCoord).rgb : vec3(1.0);
    specularTex = uHasTexture ? texture(material.texture_specular1, vertex.TexCoord).rgb : vec3(1.0);

    vec3 lighting = vec3(0.0);
    lighting += lightCalculation(diffuseTex, specularTex);
    */

    oEntityID = uEntityID;

    oColor = vec4(uColor, 1.0);
}
