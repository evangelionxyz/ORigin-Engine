// type vertex
#version 450 core
layout (location = 0) in vec3 aPositon;
layout (location = 1) in vec3 Normals;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec4 BoneIDs;
layout (location = 4) in vec4 BoneWheights;
layout (location = 5) in int EntityID;

layout(location = 0) out vec2 vUV;
layout(location = 1) out vec3 vNormals;
layout(location = 2) out vec3 vPosition;
layout(location = 3) out flat int vEntityID;

uniform mat4 boneTransforms[50];
uniform mat4 viewProjection;
uniform mat4 transform;

void main()
{
	mat4 boneTransform  =  mat4(0.0);
	boneTransform  +=    boneTransforms[int(BoneIDs.x)] * BoneWheights.x;
	boneTransform  +=    boneTransforms[int(BoneIDs.y)] * BoneWheights.y;
	boneTransform  +=    boneTransforms[int(BoneIDs.z)] * BoneWheights.z;
	boneTransform  +=    boneTransforms[int(BoneIDs.w)] * BoneWheights.w;

	vec4 pos = boneTransform * vec4(aPositon, 1.0);
	gl_Position = viewProjection * transform * pos;
	vPosition = vec3(transform * boneTransform * pos);

	vUV = UV;
	vNormals = mat3(transpose(inverse(transform * boneTransform))) * Normals;
	vNormals = normalize(vNormals);
	vEntityID = EntityID;
}

// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;
layout (location = 1) out int oEntityID;

layout(location = 0) in vec2 vUV;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec3 vPosition;
layout(location = 3) in flat int vEntityID;

uniform sampler2D uTexture;

vec3 lightPos = vec3(0.2, 1.0, -3.0);
	
void main()
{
	vec3 lightDir = normalize(lightPos - vPosition);
	float diff = max(dot(vNormals, lightDir), 0.2);
	vec3 dCol = diff * texture(uTexture, vUV).rgb;

	oColor = vec4(dCol, 1);
	oEntityID = vEntityID;
}