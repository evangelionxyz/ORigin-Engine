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
const int MAX_LIGHTS = 8;

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
};
in Vertex vertex;

struct Material
{
    sampler2D Diffuse;
    sampler2D Specular;
    float Shininess;
};
uniform Material material;

struct DirLight
{
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};
uniform DirLight directionalLight;

struct PointLight
{
    vec3 Position;
    vec3 Color;
    float Ambient;
    float Specular;
};
uniform PointLight pointLights[MAX_LIGHTS];
uniform int pointLightsCount;

struct SpotLight
{
    vec3 Position;
    vec3 Color;
    float Ambient;
    float Specular;
    float Innercone;
    float Outercone;
};
uniform SpotLight spotLight;

uniform vec3 uCameraPosition;
uniform vec4 uColor;
uniform int uEntityID;

vec3 CalcDirectionalLights();
vec3 CalcPointLights();
vec3 CalcSpotLights();

void main()
{
    oEntityID = uEntityID;

    vec3 lights = vec3(0.0);
    lights += CalcDirectionalLights();
    lights += CalcPointLights();
    lights += CalcSpotLights();

    oColor = vec4(lights, 1.0) * uColor;
}

vec3 CalcPointLights()
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for(int i; i < pointLightsCount + 1; i++)
    {
        // Ambient
        ambient += pointLights[i].Ambient * pointLights[i].Color;
    
        // Diffuse
        vec3 norm = normalize(vertex.Normal);
        vec3 lightDirection = normalize(pointLights[i].Position - vertex.Position);
        float diff = max(dot(norm, lightDirection), 0.0);
        vec3 iDiffuse = diff * pointLights[i].Color;
        diffuse += iDiffuse;

        // Specular
        vec3 viewDir = normalize(uCameraPosition - vertex.Position);
        vec3 reflectDir = reflect(-lightDirection, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 iSpecular = pointLights[i].Specular * spec * pointLights[i].Color;
        specular += iSpecular;
    }
    
    if(pointLightsCount == 1)
        return vec3(0.0);

    return diffuse + ambient + specular;
}

vec3 CalcDirectionalLights()
{
    vec3 lightDir = normalize(-directionalLight.Direction);
    float diff = max(dot(vertex.Normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, vertex.Normal);
    float spec = pow(max(dot(reflectDir, normalize(-vertex.Position)), 0.0), 32.0);

    vec3 ambient = directionalLight.Ambient;
    vec3 diffuse = directionalLight.Diffuse * diff;
    vec3 specular = directionalLight.Specular * spec;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLights()
{
    vec3 ambient = spotLight.Ambient * spotLight.Color;

    vec3 normal = normalize(vertex.Normal);
    vec3 lightDirection = normalize(spotLight.Position - vertex.Position);
    float diff = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = diff * spotLight.Color;

	vec3 viewDirection = normalize(uCameraPosition - vertex.Position);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32);
	vec3 specular = spotLight.Specular * spec * spotLight.Color;

	float angle = dot(vec3(0.0, -1.0, 0.0), -lightDirection);
	float intensity = clamp((angle - spotLight.Outercone) / (spotLight.Innercone - spotLight.Outercone), 0.0f, 1.0f);

	return (diffuse + ambient + specular) * intensity;
}