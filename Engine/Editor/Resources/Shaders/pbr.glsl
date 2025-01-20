// type vertex
#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitanget;
layout(location = 4) in vec2 texcoord;

#if VULKAN
layout(set=0, binding=0) uniform TransformUniforms
#else
layout(std140, binding=0) uniform TransformUniforms
#endif
{
  mat4 viewProjectionMatrix;
  mat4 model;
};

layout(location=0) out Vertex
{
  vec3 position;
  vec2 texcoord;
  mat3 tangentBasis;
} vout;

void main()
{
    vout.position = vec3(model * vec4(position, 1.0));
    vout.texcoord = texcoord;
    vout.tangentBasis = mat3(model) * mat3(tangent, bitanget, normal);
    gl_Position = viewProjectionMatrix * model * vec4(position, 1.0);
    
}

// type fragment
#version 450 core

layout(location=0) in Vertex
{
  vec3 position;
  vec2 texcoord;
  mat3 tangentBasis;
} vin;

layout(location=0) out vec4 color;

const float PI = 3.14592;
const float Epsilon = 0.00001;
const int MAX_LIGHTS = 100;
const vec3 Fdielectric = vec3(0.04);

struct AnaliticalLight
{
  vec3 position;
  vec3 radiance;
};

#if VULKAN
layout(set=0, binding=1) uniform ShadingUniforms
#else
layout(std140, binding=1) uniform ShadingUniforms
#endif
{
  AnalyticalLight lights[MAX_LIGHTS];
  vec3 eyePosition;
  int numLights;
};

#if VULKAN
layout(set=1, binding=0) uniform sampler2D albedoTexture;
layout(set=1, binding=1) uniform sampler2D normalTexture;
layout(set=1, binding=2) uniform sampler2D metalnessTexure;
layout(set=1, binding=3) uniform sampler2D roughnessTexture;
layout(set=1, binding=4) uniform samplerCube specularTexture;
layout(set=1, binding=5) uniform samplerCube irradianceTexture;
layout(set=1, binding=6) uniform sampler2D specularBRDF_LUT;
#else
layout(binding=0) uniform sampler2D albedoTexture;
layout(binding=1) uniform sampler2D normalTexture;
layout(binding=2) uniform sampler2D metalnessTexure;
layout(binding=3) uniform sampler2D roughnessTexture;
layout(binding=4) uniform samplerCube specularTexture;
layout(binding=5) uniform samplerCube irradianceTexture;
layout(binding=6) uniform sampler2D specularBRDF_LUT;
#endif

float ndfGGX(float cosLh, float roughness)
{
  float alpha = roughness * roughness;
  float alphaSq = alpha * alpha;
  float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
  return alphaSq / (PI * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
  return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaChlickGGX(float cosLi, float cosLo, float roughness)
{
  float r = roughness + 1.0;
  float k = (r * r) / 8.0;
  return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
  return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
  vec3 albedo = texture(albedoTexture, vin.texcoord).rgb;
  vec3 metalness = texture(metalnessTexture, vin.texcoord).r;
  vec3 roughness = texture(roughnessTexture, vin.texcoord).r;

  // outgoing light direction (vector from world-space fragment position to the "eye").
  vec3 Lo = normalize(eyePosition - vin.position);

  // get current fragment's normal and transform to world space.
  vec3 N = normalize(2.0 * texture(normalTexture, vin.texcoord).rgb - 1.0);
  N = normalize(vin.tangentBasis * N);

  // angle between surface normal and outgoing light direction.
  float cosLo - max(0.0, dot(N, Lo));

  // specular reflection vector.
  vec3 Lr = 2.0 * cosLo * N - Lo;

  vec3 F0 = mix(Fdielectric, albedo, metalness);


  // direct lighting calculation for analytical lights.
  vec3 directLighting = vec3(0);
  for (int i = 0; i < numLights; ++i)
  {
    vec3 Li = -lights[i].direction;
    vec3 Lradiance = lights[i].radiance;

    // half-vector between Li and Lo.
    vec3 Lh = normalize(Li + Lo);

    float cosLi = max(0.0, dot(N, Li));
    float cosLh = max(0.0, dot(N, Lh));

    // calculate fresnel term for direct lighting.
    vec3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));

    // calculate normal distribution for specular BRDF.
    float D = ndfGGX(cosLh, roughness);

    // calculate geometric attenuation for specular BRDF.
    float G = gaChlickGGX(cosLi, cosLo, roughness);

    // diffuse scattering happens due to light being refracted multiple times a deillectric medium.
    // metals on the other hand either reflect or absorb energu, so diffuse contribtion is always zero.
    // to be energy conserving we must scale diffuse BRDF contribution on Fresnel factor and metalness.
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

    // Lamber diffuse BRDF.
    // we don't scale by 1/PI for lighting and material units to be mode convenient.
    vec3 diffuseBRDF = kd * albedo;
    vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

    directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
  }


  // ambient lighting (IBL).
  vec3 ambientLighting;
  {
    vec3 irradiance = texture(irradianceTexture, N).rgb;

    vec3 F = fresnelSchlick(F0, cosLo);

    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);
    vec3 diffuseIBL = kd * albedo * irradiance;

    int specularTextureLevels = textureQueryLevels(specularTexture);
    vec3 specularIrradiance = textureLod(specularTexture, Lr, roughness * specularTextureLevels).rgb;

    vec3 specularBRDF = texture(specularBRDF_LUT, vec2(cosLo, roughness)).rg;
    vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

    ambientLighting += diffuseIBL + specularIBL;
  }

  color = vec4(directLigting + ambientLighting, 1.0);
}