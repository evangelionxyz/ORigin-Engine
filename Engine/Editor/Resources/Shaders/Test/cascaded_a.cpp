#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class CascadedShadowMap {
private:
    static const int NUM_CASCADES = 4;
    GLuint depthMapFBO;
    GLuint depthMapArray;
    std::vector<float> cascadeSplits;
    std::vector<glm::mat4> lightSpaceMatrices;
    
    const unsigned int SHADOW_WIDTH = 2048;
    const unsigned int SHADOW_HEIGHT = 2048;
    
public:
    CascadedShadowMap() {
        // Initialize cascade splits using practical split scheme
        cascadeSplits = calculateCascadeSplits(0.1f, 100.0f);
        
        // Create depth map FBO
        glGenFramebuffers(1, &depthMapFBO);
        
        // Create depth texture array
        glGenTextures(1, &depthMapArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, depthMapArray);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, 
                    SHADOW_WIDTH, SHADOW_HEIGHT, NUM_CASCADES, 
                    0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
        
        // Attach depth texture array to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapArray, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Framebuffer is not complete!");
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    std::vector<float> calculateCascadeSplits(float nearPlane, float farPlane) {
        std::vector<float> splits;
        float lambda = 0.5f; // Balance between uniform and logarithmic splitting
        float ratio = farPlane / nearPlane;
        
        for (int i = 0; i < NUM_CASCADES; i++) {
            float p = (i + 1) / static_cast<float>(NUM_CASCADES);
            float log = nearPlane * std::pow(ratio, p);
            float uniform = nearPlane + (farPlane - nearPlane) * p;
            float d = lambda * (log - uniform) + uniform;
            splits.push_back(d);
        }
        
        return splits;
    }
    
    void updateLightSpaceMatrices(const glm::vec3& lightDir, const glm::mat4& viewMatrix,
                                 float fov, float aspect, float nearPlane) {
        lightSpaceMatrices.clear();
        
        for (int i = 0; i < NUM_CASCADES; i++) {
            float prevSplit = (i == 0) ? nearPlane : cascadeSplits[i - 1];
            float currentSplit = cascadeSplits[i];
            
            // Calculate frustum corners for this cascade
            std::vector<glm::vec4> frustumCorners = getFrustumCorners(fov, aspect, prevSplit, currentSplit);
            
            // Calculate light view and projection matrices
            glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f), lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
            
            // Calculate bounding box in light space
            glm::vec3 minCorner(std::numeric_limits<float>::max());
            glm::vec3 maxCorner(std::numeric_limits<float>::lowest());
            
            for (const auto& corner : frustumCorners) {
                glm::vec4 lightSpaceCorner = lightView * corner;
                minCorner = glm::min(minCorner, glm::vec3(lightSpaceCorner));
                maxCorner = glm::max(maxCorner, glm::vec3(lightSpaceCorner));
            }
            
            // Create orthographic projection matrix for this cascade
            glm::mat4 lightProjection = glm::ortho(
                minCorner.x, maxCorner.x,
                minCorner.y, maxCorner.y,
                minCorner.z, maxCorner.z
            );
            
            lightSpaceMatrices.push_back(lightProjection * lightView);
        }
    }
    
    void renderShadowMap(const std::function<void(const glm::mat4&)>& renderScene) {
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // Render scene from light's perspective for each cascade
        for (int i = 0; i < NUM_CASCADES; i++) {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapArray, 0, i);
            renderScene(lightSpaceMatrices[i]);
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
private:
    std::vector<glm::vec4> getFrustumCorners(float fov, float aspect, 
                                            float nearPlane, float farPlane) {
        std::vector<glm::vec4> corners;
        float tanHalfFov = tan(glm::radians(fov * 0.5f));
        
        // Calculate frustum corners
        for (int x = 0; x < 2; ++x) {
            for (int y = 0; y < 2; ++y) {
                for (int z = 0; z < 2; ++z) {
                    float xSign = (x == 0) ? -1.0f : 1.0f;
                    float ySign = (y == 0) ? -1.0f : 1.0f;
                    float zValue = (z == 0) ? nearPlane : farPlane;
                    
                    glm::vec4 corner(
                        xSign * zValue * tanHalfFov * aspect,
                        ySign * zValue * tanHalfFov,
                        -zValue,
                        1.0f
                    );
                    corners.push_back(corner);
                }
            }
        }
        
        return corners;
    }
};