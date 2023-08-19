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

const int MAX_LIGHTS = 32;

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
};

in Vertex vertex;

uniform vec3 uCameraPosition;
uniform vec4 uColor;
uniform int uEntityID;
uniform bool uHasTexture;

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

struct PointLight
{
    vec3 Position;
    vec3 Color;
    float Ambient;
    float Specular;
};

uniform PointLight pointLights[MAX_LIGHTS];
uniform int pointLightCount;

struct SpotLight
{
    vec3 Position;
    vec3 Direction;
    vec3 Color;
    float Ambient;
    float Specular;
    float InnerCone;
    float OuterCone;
};

uniform SpotLight spotLights[MAX_LIGHTS];
uniform int spotLightCount;

vec3 CalcPointLights(vec3 normal, vec3 viewDir, vec3 diffuseTexture, vec3 specularTexture);
vec3 CalcSpotLights(vec3 normal, vec3 viewDir, vec3 diffuseTexture, vec3 specularTexture);

void main()
{
    oEntityID = uEntityID;

    vec3 diffuseTex = vec3(1.0);
    vec3 specularTex = vec3(1.0);

    if (uHasTexture)
    {
        diffuseTex = texture(material.texture_diffuse1, vertex.TexCoord).rgb;
        specularTex = texture(material.texture_specular1, vertex.TexCoord).rgb;
    }

    vec3 lights = vec3(0.0);

    vec3 normal = normalize(vertex.Normal);
    vec3 viewDirection = normalize(uCameraPosition - vertex.Position);

    lights += CalcPointLights(normal, viewDirection, diffuseTex, specularTex);
    lights += CalcSpotLights(normal, viewDirection, diffuseTex, specularTex);

    oColor = vec4(lights, 1.0) * uColor;
}

vec3 CalcPointLights(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture)
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i <= pointLightCount; i++)
    {
        // Ambient
        ambient += pointLights[i].Ambient * diffuseTexture;

        // Diffuse
        vec3 lightDirection = normalize(pointLights[i].Position - vertex.Position);
        float diff = max(dot(normal, lightDirection), 0.0);
        diffuse += diff * pointLights[i].Color * diffuseTexture;

        // Specular
        vec3 reflectionDirection = reflect(-lightDirection, normal);
        float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.Shininess);
        specular += spec * pointLights[i].Specular * specularTexture * pointLights[i].Color;
    }

    return diffuse + ambient + specular;
}

vec3 CalcSpotLights(vec3 normal, vec3 viewDirection, vec3 diffuseTexture, vec3 specularTexture)
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    float intensity = 0.0;

    for (int i = 0; i <= spotLightCount; i++)
    {
        // Ambient
        ambient += spotLights[i].Ambient * diffuseTexture;

        // Diffuse
        vec3 lightDirection = normalize(spotLights[i].Position - vertex.Position);
        float diff = max(dot(normal, lightDirection), 0.0);
        diffuse += diff * spotLights[i].Color * diffuseTexture;

        // Specular
        vec3 reflectionDirection = reflect(-lightDirection, normal);
        float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.Shininess);
        specular += spec * spotLights[i].Specular * specularTexture * spotLights[i].Color;

        // Spot Light Intensity
        float angle = dot(lightDirection, spotLights[i].Direction);
        float falloff = clamp((angle - spotLights[i].OuterCone) / (spotLights[i].InnerCone - spotLights[i].OuterCone), 0.0, 1.0);

        intensity += falloff;
    }

    return (diffuse + ambient + specular) * intensity;
}
