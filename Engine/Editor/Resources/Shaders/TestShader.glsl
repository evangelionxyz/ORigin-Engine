// type vertex
#version 450 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aUV;
layout (location = 4) in vec2 aTilingFactor;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

const int MAX_BONES          = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 viewProjection;
uniform mat4 transform;
uniform mat4 bone_transforms[MAX_BONES];

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragUV;
layout (location = 2) out vec3 fragNormal;
layout (location = 3) out vec3 fragPosition;

void main()
{
    mat4 bone_transform = bone_transforms[aBoneIDs[0]] * aWeights[0];
    bone_transform += bone_transforms[aBoneIDs[1]] * aWeights[1];
    bone_transform += bone_transforms[aBoneIDs[2]] * aWeights[2];
    bone_transform += bone_transforms[aBoneIDs[3]] * aWeights[3];

    gl_Position = viewProjection * transform * bone_transform * vec4(aPosition, 1.0);
    fragColor = aColor;
    fragUV = aUV;
    fragNormal = mat3(transpose(inverse(transform))) * aNormal; // Transform normal to world space
    fragPosition = vec3(transform * vec4(aPosition, 1.0)); // Transform position to world space
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragUV;
layout (location = 2) in vec3 fragNormal;
layout (location = 3) in vec3 fragPosition;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normals;
uniform sampler2D texture_base_color;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 lightColor;
uniform float shininess;

void main()
{
    // Ambient lighting
    vec3 ambient = 0.1 * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPosition - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    vec3 viewDir = normalize(viewPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * lightColor;

    // Combine results
    vec3 lighting = (ambient + diffuse + specular) * fragColor;

    vec4 tex_diffuse_color = texture(texture_diffuse, fragUV);
    vec4 tex_specular_color = texture(texture_specular, fragUV);
    vec4 tex_normals_color = texture(texture_normals, fragUV);
    vec4 tex_base_color = texture(texture_base_color, fragUV);

    oColor = tex_diffuse_color * vec4(lighting, 1.0);
    //oColor = tex_diffuse_color * vec4(1.0);
}