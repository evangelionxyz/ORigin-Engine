// type vertex
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoord;
layout (location = 4) in ivec4 bone_ids;
layout (location = 5) in vec4 weights;

const int MAX_BONES = 200;

layout (location = 0) out Vertex
{
  vec3 position;
  vec3 normals;
  vec3 color;
  vec2 texcoord;
  flat ivec4 bone_ids;
  vec4 weights;
} vout;

layout(std140, binding = 0) uniform Camera
{
    mat4 view_projection;
    vec3 position;
} camera_buffer;

uniform mat4 ubone_transforms[MAX_BONES];
uniform mat4 umodel_transform;
uniform bool uhas_animation;

void main()
{
    if (uhas_animation)
    {
        mat4 bone_transform = ubone_transforms[bone_ids[0]] * weights[0];
        bone_transform     += ubone_transforms[bone_ids[1]] * weights[1];
        bone_transform     += ubone_transforms[bone_ids[2]] * weights[2];
        bone_transform     += ubone_transforms[bone_ids[3]] * weights[3];

        gl_Position = camera_buffer.view_projection * umodel_transform * bone_transform * vec4(position, 1.0);

        // bone transformation for the normal (3x3 matrix, ignores translation)
        mat3 bone_normal_transform = mat3(ubone_transforms[bone_ids[0]]) * weights[0];
        bone_normal_transform     += mat3(ubone_transforms[bone_ids[1]]) * weights[1];
        bone_normal_transform     += mat3(ubone_transforms[bone_ids[2]]) * weights[2];
        bone_normal_transform     += mat3(ubone_transforms[bone_ids[3]]) * weights[3];

        // transform the normal using the combined bone normal transform
        vout.normals = mat3(transpose(inverse(umodel_transform))) * bone_normal_transform * normals;
        vout.position = vec3(umodel_transform * bone_transform * vec4(position, 1.0));
    }
    else
    {
        gl_Position = camera_buffer.view_projection * umodel_transform * vec4(position, 1.0);
        vout.normals = mat3(transpose(inverse(umodel_transform))) * normals;
        vout.position = vec3(umodel_transform * vec4(position, 1.0));
    }

    vout.color = color;
    vout.texcoord = texcoord;
    vout.bone_ids = bone_ids;
    vout.weights = weights;
}

// type fragment
#version 450 core
layout (location = 0) in Vertex
{
    vec3 position;
    vec3 normals;
    vec3 color;
    vec2 texcoord;
    flat ivec4 bone_ids;
    vec4 weights;
} vin;

struct Material
{
    vec4 base_color;
    vec4 diffuse_color;
    vec4 specular_color;
    vec2 tiling_factor;
    float roughness;
    float emisive;
};

struct SpotLight
{
    vec4 position;
    vec4 direction;
    vec4 color;
    vec2 cut_off;
};

struct PointLight
{
    vec4 position;
    vec4 intensity;
    vec4 color;
    vec4 falloff;
};

layout (location = 0) out vec4 frag_color;

/// ====================================
//          STORAGE BUFFER
/// ====================================
layout(std140, binding = 0) buffer MaterialBuffer
{
    Material materials[];
};

layout(std140, binding = 1) buffer SpotLightBuffer
{
    SpotLight spotlights[];
};

layout(std140, binding = 2) buffer PointLightBuffer
{
    PointLight pointlights[];
};

layout(std140, binding = 0) uniform Camera
{
    mat4 view_projection;
    vec3 position;
} camera_buffer;

layout(std140, binding = 1) uniform Lighting
{
    int directional_light_count;
    int spotlight_count;
    int point_light_count;
    int area_light_count;
} lighting_buffer;

layout(std140, binding = 2) uniform DirectionalLight
{
    vec4 color;     // Light light color
    vec4 direction; // light direction (should be normalized)
} dir_light_buffer;

layout(binding = 0) uniform sampler2D udiffuse_texture;
layout(binding = 1) uniform sampler2D uspecular_texture;
layout(binding = 2) uniform sampler2D uroughness_texture;
layout(binding = 3) uniform samplerCube urefract_map_cube_texture;

uniform int uspot_light_count;
uniform int umaterial_index;

Material material = materials[umaterial_index];

subroutine vec3 BRDF(in vec3, in vec3, in vec3, in vec3, in vec3, in vec3, in float);
layout(location = 0) subroutine uniform BRDF BRDFUniform;

#define M_RCPPI 0.31830988618379067153776752674503
#define M_PI 3.1415926535897932384626433832795

vec3 light_falloff(in vec3 light_intensity, in vec3 fall_off, in vec3 light_position, in vec3 position)
{
    float dist = distance(light_position, position);
    float falloff = fall_off.x + (fall_off.y * dist) + (fall_off.z * dist * dist);
    return light_intensity / falloff;
}

vec3 schlick_fresnel(in vec3 light_direction, in vec3 normal, in vec3 specular_color)
{
    float LH = dot(light_direction, normal);
    return specular_color + (1.0 - specular_color) * pow(1.0 - LH, 5.0);
}

float TRDistribution(in vec3 normal, in vec3 half_vector, in float roughness)
{
    float NSq = roughness * roughness;
    float NH = max(dot(normal, half_vector), 0.0);
    float denom = NH * NH * (NSq - 1.0) + 1.0;
    return NSq / (M_PI * denom * denom);
}

vec3 specular_transmit(in vec3 normal, in vec3 view_direction, in vec3 diffuse_color, in vec3 specular_color)
{
    float root_F0 = sqrt(specular_color.x);
    float IOR = (1.0f - root_F0) / (1.0 + root_F0);
    vec3 refract_direction = refract(view_direction, normal, IOR);
    vec3 refract_color = texture(urefract_map_cube_texture, refract_direction).rgb;
    vec3 ret_color = IOR * (1.0 - schlick_fresnel(refract_direction, -normal, specular_color));
    ret_color *= diffuse_color;
    ret_color *= refract_color;
    return ret_color;
}

vec3 texture_emisive(vec3 diffuse_color)
{
    return material.emisive * diffuse_color;
}

vec3 texture_refract_map(vec3 color_out, vec3 normal, vec3 view_direction, vec4 diffuse_color, vec3 specular_color)
{
    vec3 transmit = specular_transmit(normal, view_direction, diffuse_color.rgb, specular_color);
    return mix(transmit, color_out, diffuse_color.w);
}

float GGXVisibility(in vec3 normal, in vec3 light_direction, in vec3 view_direction, in float roughness)
{
    float NL = max(dot(normal, light_direction), 0.0);
    float NV = max(dot(normal, view_direction), 0.0);
    float RSq = roughness * roughness;
    float RMod = 1.0 - RSq;
    float recip_g1 = NL + sqrt(RSq + (RMod * NL * NL));
    float recip_g2 = NV + sqrt(RSq + (RMod * NV * NV));
    return 1.0 / (recip_g1 * recip_g2);
}

layout(index = 0) subroutine(BRDF) 
vec3 blinn_phong(in vec3 normal, in vec3 light_direction, in vec3 view_direction, in vec3 light_irradiance, in vec3 diffuse_color, in vec3 specular_color, in float roughness)
{
    vec3 diffuse = diffuse_color;
    vec3 half_vector = normalize(view_direction + light_direction);
    float roughness_phong = (2.0 / (roughness * roughness)) - 2.0;
    vec3 specular = pow(max(dot(normal, half_vector), 0.0), roughness_phong) * specular_color;
    diffuse *= M_RCPPI;
    specular *= (roughness_phong + 8.0) / (8.0 * M_PI);
    vec3 ret_color = diffuse + specular;
    ret_color *= max(dot(normal, light_direction), 0.0);
    ret_color *= light_irradiance;
    return ret_color;
}

layout(index = 1) subroutine(BRDF)
vec3 GGX(in vec3 normal, in vec3 light_direction, in vec3 view_direction, in vec3 light_irradiance, in vec3 diffuse_color, in vec3 specular_color, in float roughness)
{
    vec3 diffuse = diffuse_color * M_RCPPI;
    vec3 half_vector = normalize(view_direction + light_direction);
    vec3 F = schlick_fresnel(light_direction, half_vector, specular_color);
    float D = TRDistribution(normal, half_vector, roughness);
    float V = GGXVisibility(normal, light_direction, view_direction, roughness);
    vec3 ret_color = diffuse + (F * D * V);
    ret_color *= max(dot(normal, light_direction), 0.0);
    ret_color *= light_irradiance;
    return ret_color;
}

/// ====================================
///   DIRECTIONAL LIGHT CALCULATION
/// ====================================
vec3 calculate_directional_light(vec3 normal, vec3 view_dir, vec3 diffuse_texture_color, float shadow)
{
  vec3 light_direction = normalize(-dir_light_buffer.direction.xyz);
  vec3 normalized_normal = normalize(normal);
  vec3 normalized_view_dir = normalize(view_dir);
  float ambient_strength = 0.1;
  vec3 ambient_color = ambient_strength * dir_light_buffer.color.rgb;
  float diffuse = max(dot(normalized_normal, light_direction), 0.0);
  vec3 diffuse_color = diffuse * dir_light_buffer.color.rgb;
  float specular_strength = 0.5;
  vec3 reflect_dir = reflect(-light_direction, normalized_normal);
  float specular = pow(max(dot(normalized_view_dir, reflect_dir), 0.0), 32.0); 
  vec3 specular_color = specular_strength * specular * dir_light_buffer.color.rgb;
  return (ambient_color + (1.0 - shadow) * (diffuse_color + specular_color)) * diffuse_texture_color * material.base_color.rgb;
}

void main()
{
    vec3 diffuse_texture = texture(udiffuse_texture, vin.texcoord * material.tiling_factor.xy).rgb;
    vec3 specular_texture = texture(uspecular_texture, vin.texcoord * material.tiling_factor.xy).rgb;
    vec3 roughness_texture = texture(uroughness_texture, vin.texcoord * material.tiling_factor.xy).rgb;

    vec4 v4_diffuse_color = material.diffuse_color * texture(udiffuse_texture, vin.texcoord * material.tiling_factor.xy);
    vec3 diffuse_color = material.diffuse_color.rgb * diffuse_texture;
    vec3 specular_color = material.specular_color.rgb * specular_texture;

    vec3 normal = normalize(vin.normals);
    vec3 view_direction = normalize(camera_buffer.position - vin.position);

    vec3 lighting = vec3(0.0);
    for (int i = 0; i < lighting_buffer.point_light_count; ++i)
    {
        PointLight p = pointlights[i];

        vec3 p_light_direction = normalize(p.position.xyz - vin.position);

        vec3 p_light_irradiance = light_falloff(p.intensity.rgb, p.falloff.rgb, 
            p.position.xyz, vin.position) * p.color.rgb;

        vec3 p_light_color = BRDFUniform(normal, p_light_direction, view_direction, p_light_irradiance, diffuse_color, specular_color, material.roughness);
        lighting += p_light_color;
    }
    
    lighting += calculate_directional_light(normal, view_direction, diffuse_texture, 0.0);
    lighting += texture_emisive(diffuse_color);
    //lighting = texture_refract_map(lighting, normal, view_direction, v4_diffuse_color, specular_color);

    frag_color = vec4(lighting, 1.0);
}