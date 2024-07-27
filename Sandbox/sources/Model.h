#pragma once
#include "Origin/Core/Assert.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <string>
#include <unordered_map>

namespace origin
{
    inline glm::mat4 AssimpToGlmMatrix(aiMatrix4x4 mat)
    {
        glm::mat4 m;
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                m[x][y] = mat[y][x];
            }
        }
        return m;
    }
    inline glm::vec3 AssimpToGlmVec3(aiVector3D vec)
    {
        return glm::vec3(vec.x, vec.y, vec.z);
    }

    inline glm::quat AssimpToGlmQuat(aiQuaternion quat)
    {
        glm::quat q;
        q.x = quat.x;
        q.y = quat.y;
        q.z = quat.z;
        q.w = quat.w;

        return q;
    }

    // vertex of an animated model
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec4 boneIds = glm::vec4(0);
        glm::vec4 boneWeights = glm::vec4(0.0f);
    };

    // structure to hold bone tree (skeleton)
    struct Bone
    {
        int id = 0; // position of the bone in final upload array
        std::string name = "";
        glm::mat4 offset = glm::mat4(1.0f);
        std::vector<Bone> children = {};
    };

    // sturction representing an animation track
    struct BoneTransformTrack
    {
        std::vector<float> positionTimestamps = {};
        std::vector<float> rotationTimestamps = {};
        std::vector<float> scaleTimestamps = {};

        std::vector<glm::vec3> positions = {};
        std::vector<glm::quat> rotations = {};
        std::vector<glm::vec3> scales = {};
    };

    // structure containing animation information
    struct Animation
    {
        float duration = 0.0f;
        float ticksPerSecond = 1.0f;
        std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
    };

    // a recursive function to read all bones and form skeleton
    bool readSkeleton(Bone &boneOutput, aiNode *node, 
        std::unordered_map<std::string, std::pair<int, glm::mat4>> &boneInfoTable)
    {
        if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end())
        { // if node is actually a bone
            boneOutput.name = node->mName.C_Str();
            boneOutput.id = boneInfoTable[boneOutput.name].first;
            boneOutput.offset = boneInfoTable[boneOutput.name].second;

            for (int i = 0; i < node->mNumChildren; i++)
            {
                Bone child;
                readSkeleton(child, node->mChildren[i], boneInfoTable);
                boneOutput.children.push_back(child);
            }
            return true;
        }
        else
        { // find bones in children
            for (int i = 0; i < node->mNumChildren; i++)
            {
                if (readSkeleton(boneOutput, node->mChildren[i], boneInfoTable))
                {
                    return true;
                }

            }
        }
        return false;
    }

    void loadModel(const aiScene *scene, aiMesh *mesh, std::vector<Vertex> &verticesOutput, 
        std::vector<uint32_t> &indicesOutput, Bone &skeletonOutput, uint32_t &nBoneCount)
    {
        verticesOutput = {};
        indicesOutput = {};

        //load position, normal, uv
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = vec;

            vertex.boneIds = glm::ivec4(0);
            vertex.boneWeights = glm::vec4(0.0f);

            verticesOutput.push_back(vertex);
        }

        //load boneData to vertices
        std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfo = {};
        std::vector<uint32_t> boneCounts;
        boneCounts.resize(verticesOutput.size(), 0);
        nBoneCount = mesh->mNumBones;

        //loop through each bone
        for (uint32_t i = 0; i < nBoneCount; i++)
        {
            aiBone *bone = mesh->mBones[i];
            glm::mat4 m = AssimpToGlmMatrix(bone->mOffsetMatrix);
            boneInfo[bone->mName.C_Str()] = { i, m };

            //loop through each vertex that have that bone
            for (int j = 0; j < bone->mNumWeights; j++)
            {
                uint32_t id = bone->mWeights[j].mVertexId;
                float weight = bone->mWeights[j].mWeight;
                boneCounts[id]++;
                switch (boneCounts[id])
                {
                case 1:
                    verticesOutput[id].boneIds.x = i;
                    verticesOutput[id].boneWeights.x = weight;
                    break;
                case 2:
                    verticesOutput[id].boneIds.y = i;
                    verticesOutput[id].boneWeights.y = weight;
                    break;
                case 3:
                    verticesOutput[id].boneIds.z = i;
                    verticesOutput[id].boneWeights.z = weight;
                    break;
                case 4:
                    verticesOutput[id].boneIds.w = i;
                    verticesOutput[id].boneWeights.w = weight;
                    break;
                }
            }
        }

        //normalize weights to make all weights sum 1
        for (int i = 0; i < verticesOutput.size(); i++)
        {
            glm::vec4 &boneWeights = verticesOutput[i].boneWeights;
            float totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
            if (totalWeight > 0.0f)
            {
                verticesOutput[i].boneWeights = glm::vec4(
                    boneWeights.x / totalWeight,
                    boneWeights.y / totalWeight,
                    boneWeights.z / totalWeight,
                    boneWeights.w / totalWeight
                );
            }
        }

        //load indices
        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace &face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indicesOutput.push_back(face.mIndices[j]);
        }

        // create bone hirerchy
        readSkeleton(skeletonOutput, scene->mRootNode, boneInfo);
    }

    void LoadAnimation(const aiScene *scene, Animation &animation)
    {
        // Loading the first Animation
        aiAnimation *anim = scene->mAnimations[0];

        if (anim->mTicksPerSecond != 0.0f)
            animation.ticksPerSecond = anim->mTicksPerSecond;
        else
            animation.ticksPerSecond = 1.0f;

        animation.duration = anim->mDuration;
        animation.boneTransforms = {};

        // Load positions, rotations, and scales for each bone
        // Each channel represents a bone
        for (unsigned int i = 0; i < anim->mNumChannels; ++i)
        {
            aiNodeAnim *channel = anim->mChannels[i];
            BoneTransformTrack track;
            for (unsigned int j = 0; j < channel->mNumPositionKeys; ++j)
            {
                track.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
                track.positions.push_back(AssimpToGlmVec3(channel->mPositionKeys[j].mValue));
            }
            for (unsigned int j = 0; j < channel->mNumRotationKeys; ++j)
            {
                track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
                track.rotations.push_back(AssimpToGlmQuat(channel->mRotationKeys[j].mValue));
            }
            for (unsigned int j = 0; j < channel->mNumScalingKeys; ++j)
            {
                track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
                track.scales.push_back(AssimpToGlmVec3(channel->mScalingKeys[j].mValue));
            }
            animation.boneTransforms[channel->mNodeName.C_Str()] = track;
        }
    }


    std::pair<uint32_t, float> GetTimeFraction(std::vector<float> &times, float &dt)
    {
        uint32_t segment = 0;
        while (segment < times.size() && dt > times[segment])
        {
            segment++;
        }
        if (segment == 0)
        {
            return { segment, 0.0f }; // dt is before the first timestamp
        }
        else if (segment >= times.size())
        {
            return { (uint32_t)times.size() - 1, 1.0f }; // dt is after the last timestamp
        }

        float start = times[segment - 1];
        float end = times[segment];
        float frac = (dt - start) / (end - start);
        return { segment, frac };
    }

    void GetPose(Animation &animation, Bone &skeleton, float timeMS, std::vector<glm::mat4> &output, glm::mat4 &parentTransform, glm::mat4 &globalInverseTransform)
    {
        BoneTransformTrack &btt = animation.boneTransforms[skeleton.name];
        timeMS = fmod(timeMS, animation.duration);
        std::pair<uint32_t, float> fp;

        // Calculate interpolated position
        fp = GetTimeFraction(btt.positionTimestamps, timeMS);
        glm::vec3 position1 = btt.positions[fp.first - 1];
        glm::vec3 position2 = btt.positions[fp.first];
        glm::vec3 position = glm::mix(position1, position2, fp.second);

        // Calculate interpolated rotation
        fp = GetTimeFraction(btt.rotationTimestamps, timeMS);
        glm::quat rotation1 = btt.rotations[fp.first - 1];
        glm::quat rotation2 = btt.rotations[fp.first];
        glm::quat rotation = glm::slerp(rotation1, rotation2, fp.second);

        // Calculate interpolated scale
        fp = GetTimeFraction(btt.scaleTimestamps, timeMS);
        glm::vec3 scale1 = btt.scales[fp.first - 1];
        glm::vec3 scale2 = btt.scales[fp.first];
        glm::vec3 scale = glm::mix(scale1, scale2, fp.second);

        glm::mat4 positionMat = glm::mat4(1.0f);
        glm::mat4 scaleMat = glm::mat4(1.0f);

        // Calculate localTransform
        positionMat = glm::translate(positionMat, position);
        glm::mat4 rotationMat = glm::toMat4(rotation);
        scaleMat = glm::scale(scaleMat, scale);
        glm::mat4 localTransform = positionMat * rotationMat * scaleMat;
        glm::mat4 globalTransform = parentTransform * localTransform;

        output[skeleton.id] = globalInverseTransform * globalTransform * skeleton.offset;
        // Update values for children bones
        for (Bone &child : skeleton.children)
        {
            GetPose(animation, child, timeMS, output, globalTransform, globalInverseTransform);
        }
    }


    inline unsigned int createShader(const char *vertexStr, const char *fragmentStr)
    {
        int success;
        char info_log[512];
        uint32_t
            program = glCreateProgram(),
            vShader = glCreateShader(GL_VERTEX_SHADER),
            fShader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vShader, 1, &vertexStr, 0);
        glCompileShader(vShader);
        glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vShader, 512, 0, info_log);
            OGN_CORE_ASSERT(false, "info_log");
        }
        glShaderSource(fShader, 1, &fragmentStr, 0);
        glCompileShader(fShader);
        glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fShader, 512, 0, info_log);
            OGN_CORE_ASSERT(false, info_log);
        }

        glAttachShader(program, vShader);
        glAttachShader(program, fShader);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 512, 0, info_log);
            OGN_CORE_ASSERT(false, info_log);
        }
        glDetachShader(program, vShader);
        glDeleteShader(vShader);
        glDetachShader(program, fShader);
        glDeleteShader(fShader);

        return program;
    }
}