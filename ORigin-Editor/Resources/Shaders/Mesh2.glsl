// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

// type vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uViewProjection;
uniform mat4 uModelTransform;
uniform mat4 uLightSpaceMatrix;

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
    vec4 LightSpacePosition;
};
out Vertex vertex;

void main()
{
    vertex.Position = vec3(uModelTransform * vec4(aPos, 1.0));
    vertex.Normal = mat3(transpose(inverse(uModelTransform))) * aNormal;
	  vertex.TexCoord = aTexCoord;

    vertex.LightSpacePosition = uLightSpaceMatrix * vec4(vertex.Position, 1.0);
	  gl_Position = uViewProjection * vec4(vertex.Position, 1.0);
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

uniform int uEntityID;
uniform vec4 uColor;
uniform vec3 uCameraPosition;
uniform bool uHasTextures;

const int MAX_LIGHTS = 32;

struct Vertex {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;

    vec4 LightSpacePosition;
};
in Vertex vertex;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
    sampler2D texture_specular2;

    sampler2D shadow_map;

    float Shininess;
    float Bias;
    vec2 TilingFactor;
};
uniform Material material;

float ShadowCalculation(vec4 lightSpacePosition, vec3 lightDirection) {
    vec3 projectionCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projectionCoords = projectionCoords * 0.5 + 0.5;

    vec3 normal = normalize(vertex.Normal);
    vec3 lightDir = normalize(lightDirection - vertex.Position);

    float bias = max(material.Bias * (1.0 - dot(normal, lightDir)), 0.000001);
    vec2 texelSize = 1.0 / textureSize(material.shadow_map, 0);
    float shadow = 0.0;

    float currentDepth = projectionCoords.z;

    if (currentDepth <= 1.0)
    {
        int rep = 1;
        for (int x = -rep; x <= rep; x++)
        {
            for (int y = -rep; y <= rep; y++)
            {
                vec2 offset = vec2(x, y) * texelSize;
                float pcfDepth = texture(material.shadow_map, projectionCoords.xy + offset).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;
    }

    return shadow;
}

struct PointLight {
    vec3 Position;
    vec3 Color;
    float Ambient;
    float Specular;
    float Intensity;
    float SpreadSize;
};
uniform PointLight pointLights[MAX_LIGHTS];
uniform int pointLightCount;

struct SpotLight {
    vec3 Position;
    vec3 Direction;
    vec3 Color;
    float InnerConeAngle;
    float OuterConeAngle;
    float Exponent;
};
uniform SpotLight spotLights[MAX_LIGHTS];
uniform int spotLightCount;

struct DirLight {
    vec3 Direction;
    vec3 Color;
    float Ambient;
    float Diffuse;
    float Specular;
};
uniform DirLight dirLight;

vec3 CalcPointLights(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture);
vec3 CalcSpotLights(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture);
vec3 CalcDirLight(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture);

void main()
{
    oEntityID = uEntityID;

    vec3 diffuseTex = vec3(1.0);
    vec3 specularTex = vec3(1.0);

    if (uHasTextures)
    {
        vec2 texTiling = vec2(vertex.TexCoord * material.TilingFactor);
        diffuseTex = texture(material.texture_diffuse1, texTiling).rgb;
        specularTex = texture(material.texture_specular1, texTiling).rgb;
    }

    vec3 lights = vec3(0.0);
    vec3 normal = normalize(vertex.Normal);
    vec3 viewDirection = normalize(uCameraPosition - vertex.Position);

    lights += CalcPointLights(normal, viewDirection, diffuseTex, specularTex);
    lights += CalcSpotLights(normal, viewDirection, diffuseTex, specularTex);
    lights += CalcDirLight(normal, viewDirection, diffuseTex, specularTex);

    vec3 fragColor = lights * uColor.rgb;
    float gamma = 2.2;

    oColor = vec4(pow(fragColor, vec3(1.0/gamma)), 1.0);
}

vec3 CalcPointLights(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture)
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    float totalIntensity = 0.0; // Accumulate total intensity for normalization

    for (int i = 0; i <= pointLightCount; i++)
    {
        ambient += pointLights[i].Ambient * pointLights[i].Color * diffuseTexture;

        vec3 lightDirection = normalize(pointLights[i].Position - vertex.Position);
        float distanceToLight = length(pointLights[i].Position - vertex.Position);

        float attenuation = (1.0 / (1.0 +pow(distanceToLight, 2.0))) * pointLights[i].SpreadSize;

        float diff = max(dot(normal, lightDirection), 0.0);
        diffuse += diff * attenuation * diffuseTexture * pointLights[i].Color;

        vec3 reflectionDirection = reflect(-lightDirection, normal);
        float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.Shininess);
        specular += spec * attenuation * pointLights[i].Specular * specularTexture * pointLights[i].Color;

        totalIntensity += pointLights[i].Intensity / dot((pointLights[i].Position - vertex.Position), (pointLights[i].Position - vertex.Position));
    }

    vec3 finalColor = (diffuse + ambient + specular) / max(totalIntensity, 1.0);

    return finalColor;
}

vec3 CalcSpotLights(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture)
{
    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i <= spotLightCount; i++)
    {
        vec3 lightDirection = normalize(spotLights[i].Position - vertex.Position);
        float attenuation = 1.0 / length(spotLights[i].Position - vertex.Position);

        vec3 spotlightDirection = normalize(spotLights[i].Direction);
        float spotCosine = dot(-lightDirection, spotlightDirection);
        float innerCosine = cos(spotLights[i].InnerConeAngle * 0.5);
        float outerCosine = cos(spotLights[i].OuterConeAngle * 0.5);

        float falloff = smoothstep(outerCosine, innerCosine, spotCosine);

        if (spotCosine > outerCosine)
        {
            float diffuseFactor = max(dot(normal, lightDirection), 0.0);
            vec3 diffuse = diffuseFactor * spotLights[i].Color * diffuseTexture;
            vec3 reflection = reflect(-lightDirection, normal);
            float specularFactor = pow(max(dot(reflection, viewDirection), 0.0), spotLights[i].Exponent);
            vec3 specular = specularFactor * spotLights[i].Color * specularTexture;
            diffuse *= attenuation * falloff;
            specular *= attenuation * falloff;
            totalDiffuse += diffuse;
            totalSpecular += specular;
        }
    }

    return totalDiffuse + totalSpecular;
}

vec3 CalcDirLight(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture)
{
    vec3 lightDirection = normalize(dirLight.Direction);

    vec3 reflectDirection = reflect(-lightDirection, normal);
    vec3 ambient = dirLight.Ambient * dirLight.Color * diffuseTexture;

    float diffuseContrib = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diffuseContrib * dirLight.Diffuse * dirLight.Color * diffuseTexture;

    float specularContrib = pow(max(dot(viewDirection, reflectDirection), 0.0), material.Shininess);
    vec3 specular = dirLight.Specular * specularContrib * dirLight.Color * specularTexture;

    float shadow = ShadowCalculation(vertex.LightSpacePosition, dirLight.Direction);
    return ambient + (1.0 - shadow) * (diffuse + specular);
}
