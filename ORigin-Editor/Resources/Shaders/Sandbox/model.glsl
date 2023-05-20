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

struct Lighting
{
    vec3 Position;
    vec3 Color;
    float Ambient;
    float Specular;
};
const int MAX_LIGHTS = 8;
uniform Lighting lights[MAX_LIGHTS];

uniform vec3 uCameraPosition;
uniform vec4 uColor;
uniform int uEntityID;

vec3 CalcLight()
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        // Ambient
        ambient = lights[i].Ambient * lights[i].Color;
    
        // Diffuse 
        vec3 norm = normalize(vertex.Normal);
        vec3 lightDirection = normalize(lights[i].Position - vertex.Position);
        float diff = max(dot(norm, lightDirection), 0.0);
        vec3 iDiffuse = diff * lights[i].Color;
        diffuse += iDiffuse;

        // Specular
        vec3 viewDir = normalize(uCameraPosition - vertex.Position);
        vec3 reflectDir = reflect(-lightDirection, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 iSpecular = lights[i].Specular * spec * lights[i].Color;
        specular += iSpecular;
    }

    return diffuse + ambient + specular;
}

void main()
{
    oEntityID = uEntityID;
    oColor = vec4(CalcLight(), 1.0) * uColor;
}