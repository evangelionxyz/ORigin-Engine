// type vertex
#version 450 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normals;
layout (location = 2) in vec2 UV;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

const int MAX_BONES          = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 viewProjection;
uniform mat4 model;
uniform mat4 boneTransforms[100];

layout(location = 0) out vec2 vUV;
layout(location = 1) out vec3 vNormals;
layout(location = 2) out vec3 vPosition;

void main()
{

	//vec4 totalPosition = vec4(0.0);
    //vec3 totalNormal = vec3(0.0);
    //
    //for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    //{
    //    if(BoneIDs[i] == -1)
    //    {
    //        continue;
    //    }
    //    
    //    if(BoneIDs[i] >= MAX_BONES)
    //    {
    //        totalPosition = vec4(Position, 1.0);
    //        totalNormal = Normals;
    //        break;
    //    }
    //
    //    vec4 localPosition = boneTransforms[BoneIDs[i]] * vec4(Position, 1.0);
    //    totalPosition += localPosition * Weights[i];
    //
    //    vec3 localNormal = mat3(transpose(inverse(boneTransforms[BoneIDs[i]]))) * Normals;
    //    totalNormal += localNormal * Weights[i];
    //}
    //
    //gl_Position = viewProjection * model * totalPosition;
    //vPosition = vec3(model * vec4(Position, 1.0));
    //vUV = UV;
    //vNormals = normalize(totalNormal);

	mat4 boneTransform = boneTransforms[BoneIDs[0]] * Weights[0];
	boneTransform     += boneTransforms[BoneIDs[1]] * Weights[1];
	boneTransform     += boneTransforms[BoneIDs[2]] * Weights[2];
	boneTransform     += boneTransforms[BoneIDs[3]] * Weights[3];

	vec4 pos = boneTransform * vec4(Position, 1.0);
    gl_Position = viewProjection * model * pos;
    vPosition = vec3(model * pos);
    vUV = UV;
    vNormals = normalize(Normals);
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;

layout(location = 0) in vec2 vUV;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec3 vPosition;

uniform sampler2D uTexture;
	
vec3 lightPosisiton = vec3(0.0, -8.0, 0.0);
void main()
{
	vec3 lightDirection = normalize(lightPosisiton - vPosition);
	float diffuse = max(dot(vNormals, lightDirection), 0.2);
	vec3 dColor = diffuse * texture(uTexture, vUV).rgb;
	oColor = vec4(dColor, 1.0);
}