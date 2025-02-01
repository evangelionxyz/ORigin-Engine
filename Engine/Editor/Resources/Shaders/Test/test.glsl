// Shadow mapping vertex shader
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};
uniform mat4 model;
uniform mat4 lightSpaceMatrices[4];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[4];
} vs_out;

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    
    // Calculate fragment position in light space for each cascade
    for(int i = 0; i < 4; ++i)
        vs_out.FragPosLightSpace[i] = lightSpaceMatrices[i] * vec4(vs_out.FragPos, 1.0);
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

// Shadow mapping fragment shader
#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[4];
} fs_in;

uniform sampler2DArray shadowMap;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform float cascadePlaneDistances[4];
uniform float shadowBias[4];  // Different bias for each cascade

// PCF kernel size (adjust for quality vs performance)
const int pcfSize = 3;
const float totalSamples = (pcfSize * 2.0 + 1.0) * (pcfSize * 2.0 + 1.0);

// Function to determine which cascade to use
int getCascadeLevel(float fragDepth) {
    for(int i = 0; i < 4; i++) {
        if(fragDepth < cascadePlaneDistances[i])
            return i;
    }
    return 3;
}

// Advanced PCF sampling with variable kernel size
float calculateShadow(vec3 fragPos, vec3 normal, int cascadeIndex) {
    vec4 fragPosLightSpace = fs_in.FragPosLightSpace[cascadeIndex];
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // Calculate bias based on surface angle and cascade level
    float bias = max(shadowBias[cascadeIndex] * 
                    (1.0 - dot(normal, lightDir)), 
                    shadowBias[cascadeIndex] * 0.005);
    
    // PCF sampling
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;
    
    for(int x = -pcfSize; x <= pcfSize; ++x) {
        for(int y = -pcfSize; y <= pcfSize; ++y) {
            float pcfDepth = texture(shadowMap, 
                                   vec3(projCoords.xy + vec2(x, y) * texelSize,
                                   float(cascadeIndex))).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    
    // Apply smooth transition between cascades
    float fadeStart = cascadePlaneDistances[cascadeIndex] - 
                     (cascadePlaneDistances[cascadeIndex] * 0.1);
    float fadeEnd = cascadePlaneDistances[cascadeIndex];
    float fadeLength = fadeEnd - fadeStart;
    float fade = 1.0 - clamp((currentDepth - fadeStart) / fadeLength, 0.0, 1.0);
    
    return (shadow / totalSamples) * fade;
}

void main() {
    // Calculate basic lighting
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    
    // Ambient
    vec3 ambient = 0.2 * color;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Determine cascade level based on fragment depth
    float viewDistance = length(viewPos - fs_in.FragPos);
    int cascadeIndex = getCascadeLevel(viewDistance);
    
    // Calculate shadow
    float shadow = calculateShadow(fs_in.FragPos, normal, cascadeIndex);
    
    // Final color
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * color;
    
    // Visualize cascade levels (for debugging)
    #ifdef DEBUG_CASCADES
    if(cascadeIndex == 0)
        lighting *= vec3(1.0, 0.9, 0.9);
    else if(cascadeIndex == 1)
        lighting *= vec3(0.9, 1.0, 0.9);
    else if(cascadeIndex == 2)
        lighting *= vec3(0.9, 0.9, 1.0);
    else
        lighting *= vec3(1.0, 0.9, 1.0);
    #endif
    
    FragColor = vec4(lighting, 1.0);
}

// Add these optimizations to the CascadedShadowMap class:
class CascadedShadowMap {
    // ... (previous code remains the same)

    void optimizeCascades() {
        // Adjust resolution for each cascade
        const std::vector<int> cascadeResolutions = {
            4096,  // Cascade 0 (nearest)
            2048,  // Cascade 1
            1024,  // Cascade 2
            512    // Cascade 3 (farthest)
        };
        
        // Recreate depth map array with varying resolutions
        glDeleteTextures(1, &depthMapArray);
        glGenTextures(1, &depthMapArray);
        
        for (int i = 0; i < NUM_CASCADES; i++) {
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
                        cascadeResolutions[i], cascadeResolutions[i], 1,
                        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
        
        // Update shadow bias for each cascade
        shadowBias = {
            0.00025f,  // Nearest cascade (small bias)
            0.00075f,  // Mid-near cascade
            0.0025f,   // Mid-far cascade
            0.005f     // Farthest cascade (larger bias)
        };
    }
    
    void setOptimalFiltering() {
        glBindTexture(GL_TEXTURE_2D_ARRAY, depthMapArray);
        
        // Enable hardware PCF
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, 
                       GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        
        // Enable anisotropic filtering if available
        float maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 
                       maxAniso);
        
        // Enable mipmapping for distant shadows
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, 
                       GL_LINEAR_MIPMAP_LINEAR);
    }
};