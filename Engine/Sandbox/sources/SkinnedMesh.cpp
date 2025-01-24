#include "SkinnedMesh.hpp"

#include <glad/glad.h>

#include "Origin/Core/Assert.h"
#include "Origin/Math/Math.h"
#include <glm/gtx/quaternion.hpp>

using namespace origin;

SkinnedMesh::~SkinnedMesh()
{
}

bool SkinnedMesh::LoadMesh(const std::string &filename)
{
    bool ret_val = false;
    m_Scene = m_Importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);
    if (m_Scene)
    {
        m_GlobalInverseTransform = Math::AssimpToGlmMatrix(m_Scene->mRootNode->mTransformation);
        m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);

        ret_val = InitFromScene(m_Scene, filename);
    }
    else
    {
        OGN_CORE_ASSERT(false, "Error parsing {}: '%s'\n", filename, m_Importer.GetErrorString());
    }

    return ret_val;
}

void SkinnedMesh::Render()
{
#if 0
    glBindVertexArray(m_VAO);

    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        glDrawElementsBaseVertex(GL_TRIANGLES,
            m_Meshes[i].num_indices,
            GL_UNSIGNED_INT,
            (void *)(sizeof(u32) * m_Meshes[i].base_index),
            m_Meshes[i].base_vertex);
    }

    glBindVertexArray(0);
#endif
}

void SkinnedMesh::GetBoneTransforms(f32 time_in_sec, std::vector<glm::mat4> &transforms, u32 anim_index)
{
    glm::mat4 identity = glm::mat4(1.0f);

    f32 ticks_persecond = static_cast<float>(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f);
    f32 time_in_ticks = time_in_sec * ticks_persecond;
    f32 animation_time_ticks = fmod(time_in_ticks, static_cast<float>(m_Scene->mAnimations[anim_index]->mDuration));

    ReadNodeHierarchy(animation_time_ticks, m_Scene->mRootNode, identity);

    transforms.resize(m_BoneInfo.size());

    for (size_t i = 0; i < m_BoneInfo.size(); ++i)
    {
        transforms[i] = m_BoneInfo[i].final_transformation;
    }
}

void SkinnedMesh::GetBoneTransfromsBlended(f32 time_in_sec, std::vector<glm::mat4> &blended_transforms, u32 start_anim_index, u32 end_anim_index, f32 blend_factor)
{
    OGN_CORE_ASSERT(start_anim_index < m_Scene->mNumAnimations && end_anim_index < m_Scene->mNumAnimations, 
        "Invalid start animation index %d, max is %d", start_anim_index, m_Scene->mNumAnimations);
    OGN_CORE_ASSERT(blend_factor >= 0.0f && blend_factor <= 1.0f, "Invalid blend factor %.2f", blend_factor);
    f32 start_anim_ticks = CalcAnimationTimeTicks(time_in_sec, start_anim_index);
    f32 end_anim_ticks = CalcAnimationTimeTicks(time_in_sec, end_anim_index);

    const aiAnimation *start_anim = m_Scene->mAnimations[start_anim_index];
    const aiAnimation *end_anim = m_Scene->mAnimations[end_anim_index];

    glm::mat4 identity(1.0f);

    ReadNodeHierarchyBlended(start_anim_ticks, end_anim_ticks, m_Scene->mRootNode,
        identity, start_anim, end_anim, blend_factor);

    blended_transforms.resize(m_Bones.size());

    for (size_t i = 0; i < m_Bones.size(); ++i)
    {
        blended_transforms[i] = m_BoneInfo[i].final_transformation;
    }
}   

bool SkinnedMesh::IsAnimated() const
{
    bool ret = m_Scene->mNumAnimations > 0;

    if (ret && (NumBones() == 0))
    {
        OGN_CORE_ASSERT(false, "Animation without bones?");
    }

    return ret;
}

bool SkinnedMesh::InitFromScene(const aiScene *scene, const std::string &filename)
{
    m_Meshes.resize(scene->mNumMeshes);
    InitializeRequiredNodeMap(m_Scene->mRootNode);

    u32 num_vertices = 0;
    u32 num_indices = 0;
    CountVerticesAndIndices(scene, num_vertices, num_indices);
    ReserveSpace(num_vertices, num_indices);
    InitAllMeshes(scene);
    return true;
}

void SkinnedMesh::CountVerticesAndIndices(const aiScene *scene, u32 &num_vertices, u32 &num_indices)
{
    for (u32 i = 0; i < m_Meshes.size(); ++i)
    {
        m_Meshes[i].mesh_entry.material_index = scene->mMeshes[i]->mMaterialIndex;
        m_Meshes[i].mesh_entry.num_indices = scene->mMeshes[i]->mNumFaces * 3;
        m_Meshes[i].mesh_entry.base_vertex = num_vertices;
        m_Meshes[i].mesh_entry.base_index = num_indices;

        num_vertices += scene->mMeshes[i]->mNumVertices;
        num_indices += m_Meshes[i].mesh_entry.num_indices;
    }
}

void SkinnedMesh::ReserveSpace(u32 num_vertices, u32 num_indices)
{
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        m_Meshes[i].vertices.reserve(num_vertices);
        m_Meshes[i].indices.reserve(num_indices);
    }

    m_Bones.resize(num_vertices);
}

void SkinnedMesh::InitAllMeshes(const aiScene *scene)
{
    for (u32 mesh_index = 0; mesh_index < m_Meshes.size(); ++mesh_index)
    {
        const aiMesh *mesh = scene->mMeshes[mesh_index];
        InitSingleMesh(mesh_index, mesh);
    }
}

void SkinnedMesh::InitSingleMesh(u32 mesh_index, const aiMesh *mesh)
{
    const aiVector3D zero_vec3(0.0f, 0.0f, 0.0f);

    SkinnedVertex v;

    for (u32 i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D &pos = mesh->mVertices[i];
        v.position = { pos.x, pos.y, pos.z };

        if (mesh->mNormals)
        {
            const aiVector3D &normal = mesh->mNormals[i];
            v.normal = { normal.x, normal.y, normal.z };
        }
        else
        {
            aiVector3D normal(0.0f, 1.0f, 0.0f);
            v.normal = { normal.x, normal.y, normal.z };
        }

        const aiVector3D &texcoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero_vec3;
        v.texcoord = { texcoord.x, texcoord.y };

        m_Meshes[mesh_index].vertices.push_back(v);
    }

    // populate index buffer
    for (u32 i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace &face = mesh->mFaces[i];
        m_Meshes[mesh_index].indices.push_back(face.mIndices[0]);
        m_Meshes[mesh_index].indices.push_back(face.mIndices[1]);
        m_Meshes[mesh_index].indices.push_back(face.mIndices[2]);
    }

    LoadMeshBones(mesh_index, m_Meshes[mesh_index].vertices, mesh);

    static origin::BufferLayout layout =
    {
        { ShaderDataType::Float3, "position" },
        { ShaderDataType::Float2, "texcoord" },
        { ShaderDataType::Float3, "normals"  },
        { ShaderDataType::Int4,   "bone_ids" },
        { ShaderDataType::Float4, "weights"  },
    };

    CreateVertex<SkinnedVertex>(m_Meshes[mesh_index].vertices, m_Meshes[mesh_index].vertex_array, m_Meshes[mesh_index].vertex_buffer, m_Meshes[mesh_index].indices, layout);

}

void SkinnedMesh::LoadMeshBones(u32 mesh_index, std::vector<SkinnedVertex> &skinned_vertices, const aiMesh *mesh)
{
    for (u32 i = 0; i < mesh->mNumBones; ++i)
    {
        LoadSingleBone(mesh_index, skinned_vertices, mesh->mBones[i]);
    }
}

void SkinnedMesh::LoadSingleBone(u32 mesh_index, std::vector<SkinnedVertex> &skinned_vertices, const aiBone *bone)
{
    i32 bone_id = GetBoneId(bone);

    if (bone_id == m_BoneInfo.size())
    {
        BoneInfoA bi(origin::Math::AssimpToGlmMatrix(bone->mOffsetMatrix));
        m_BoneInfo.push_back(bi);
    }

    for (u32 i = 0; i < bone->mNumWeights; ++i)
    {
        const aiVertexWeight &vertex_weight = bone->mWeights[i];
        const u32 global_vertex_id = m_Meshes[mesh_index].mesh_entry.base_vertex + bone->mWeights[i].mVertexId;
        m_Bones[global_vertex_id].AddBoneData(bone_id, vertex_weight.mWeight);
    }

    MarkRequiredNodesForBone(bone);
}

i32 SkinnedMesh::GetBoneId(const aiBone *bone)
{
    std::string bone_name(bone->mName.C_Str());
    if (m_BoneNameToIndexMap.find(bone_name) == m_BoneNameToIndexMap.end())
    {
        const i32 bone_index = static_cast<i32>(m_BoneNameToIndexMap.size());
        m_BoneNameToIndexMap[bone_name] = bone_index;
        return bone_index;
    }
    
    return m_BoneNameToIndexMap[bone_name];
}

void SkinnedMesh::CalcInterpolatedScaling(aiVector3D &out, f32 anim_time, const aiNodeAnim *node_anim)
{
    if (node_anim->mNumScalingKeys == 1)
    {
        out = node_anim->mScalingKeys[0].mValue;
        return;
    }

    const u32 scaling_index = FindScaling(anim_time, node_anim);
    const u32 next_scaling_index = scaling_index + 1;
    const f32 t1 = static_cast<f32>(node_anim->mScalingKeys[scaling_index].mTime);
    const f32 t2 = static_cast<f32>(node_anim->mScalingKeys[next_scaling_index].mTime);
    const f32 delta_time = t2 - t1;
    f32 factor = (anim_time - t1) / delta_time;

    const aiVector3D &start = node_anim->mScalingKeys[scaling_index].mValue;
    const aiVector3D &end = node_anim->mScalingKeys[next_scaling_index].mValue;
    const aiVector3D delta = end - start;

    out = start + factor * delta;
}

void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion &out, f32 anim_time, const aiNodeAnim *node_anim)
{
    if (node_anim->mNumRotationKeys == 1)
    {
        out = node_anim->mRotationKeys[0].mValue;
        return;
    }

    const u32 rotation_index = FindRotation(anim_time, node_anim);
    const u32 next_rotation_index = rotation_index + 1;
    const f32 t1 = static_cast<f32>(node_anim->mRotationKeys[rotation_index].mTime);
    const f32 t2 = static_cast<f32>(node_anim->mRotationKeys[next_rotation_index].mTime);
    const f32 delta_time = t2 - t1;
    f32 factor = (anim_time - t1) / delta_time;

    const aiQuaternion &start = node_anim->mRotationKeys[rotation_index].mValue;
    const aiQuaternion &end = node_anim->mRotationKeys[next_rotation_index].mValue;
    aiQuaternion::Interpolate(out, start, end, factor);
    out = start;
    out.Normalize();
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D &out, f32 anim_time, const aiNodeAnim *node_anim)
{
    if (node_anim->mNumPositionKeys == 1)
    {
        out = node_anim->mPositionKeys[0].mValue;
        return;
    }

    const u32 position_index = FindPosition(anim_time, node_anim);
    const u32 next_position_index = position_index + 1;
    const f32 t1 = static_cast<f32>(node_anim->mPositionKeys[position_index].mTime);
    const f32 t2 = static_cast<f32>(node_anim->mPositionKeys[next_position_index].mTime);
    const f32 delta_time = t2 - t1;
    f32 factor = (anim_time - t1) / delta_time;

    const aiVector3D &start = node_anim->mPositionKeys[position_index].mValue;
    const aiVector3D &end = node_anim->mPositionKeys[next_position_index].mValue;
    const aiVector3D delta = end - start;

    out = start + factor * delta;
}

u32 SkinnedMesh::FindScaling(f32 anim_time, const aiNodeAnim *node_anim)
{
    for (u32 i = 0; i < node_anim->mNumScalingKeys - 1; ++i)
    {
        f32 t = static_cast<f32>(node_anim->mScalingKeys[i + 1].mTime);
        if (anim_time < t)
            return i;
    }
    return 0;
}

u32 SkinnedMesh::FindRotation(f32 anim_time, const aiNodeAnim *node_anim)
{
    for (u32 i = 0; i < node_anim->mNumRotationKeys - 1; ++i)
    {
        f32 t = static_cast<f32>(node_anim->mRotationKeys[i + 1].mTime);
        if (anim_time < t)
            return i;
    }
    return 0;
}

u32 SkinnedMesh::FindPosition(f32 anim_time, const aiNodeAnim *node_anim)
{
    for (u32 i = 0; i < node_anim->mNumPositionKeys - 1; ++i)
    {
        f32 t = static_cast<f32>(node_anim->mPositionKeys[i + 1].mTime);
        if (anim_time < t)
            return i;
    }
    return 0;
}

const aiNodeAnim *SkinnedMesh::FindNodeAnim(const aiAnimation *animation, const std::string &node_name)
{
    for (u32 i = 0; i < animation->mNumChannels; ++i)
    {
        const aiNodeAnim *node_anim = animation->mChannels[i];
        if (std::string(node_anim->mNodeName.data) == node_name)
            return node_anim;
    }
    return nullptr;
}

void SkinnedMesh::ReadNodeHierarchy(f32 time_in_ticks, const aiNode *node, const glm::mat4 &parent_transform)
{
    std::string node_name(node->mName.C_Str());

    glm::mat4 node_transformation = Math::AssimpToGlmMatrix(node->mTransformation);

    const aiAnimation *animation = m_Scene->mAnimations[0];

    const aiNodeAnim *node_anim = FindNodeAnim(animation, node_name);
    if (node_anim)
    {
        LocalTransform transform;
        CalcLocalTransform(transform, time_in_ticks, node_anim);

        glm::mat4 scale_transform = glm::scale(glm::mat4(1.0f), { transform.scaling.x, transform.scaling.y, transform.scaling.z });
        glm::quat rotation_transform = Math::AssimpToGlmQuat(transform.rotation);
        glm::mat4 translation_transform = glm::translate(glm::mat4(1.0f), { transform.translation.x, transform.translation.y, transform.translation.z });
        node_transformation = translation_transform * glm::toMat4(rotation_transform) * scale_transform;
    }
    
    glm::mat4 global_transform = parent_transform * node_transformation;
    if (m_BoneNameToIndexMap.contains(node_name))
    {
        const u32 bone_index = m_BoneNameToIndexMap[node_name];
        m_BoneInfo[bone_index].final_transformation = m_GlobalInverseTransform * global_transform * m_BoneInfo[bone_index].offset_matrix;
    }

    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        std::string child_name(node->mChildren[i]->mName.C_Str());
        auto it = m_RequiredNodeMap.find(child_name);
        if (it == m_RequiredNodeMap.end())
        {
            OGN_CORE_ASSERT(false, "Child {} cannot be found in the required node map", child_name);
        }
        if (it->second.is_requiered)
        {
            ReadNodeHierarchy(time_in_ticks, node->mChildren[i], global_transform);
        }
    }
}

void SkinnedMesh::ReadNodeHierarchyBlended(f32 start_anim_time_ticks, f32 end_anim_time_ticks, const aiNode *node, const glm::mat4 &parent_transform, 
    const aiAnimation *start_animation, const aiAnimation *end_animation, f32 blend_factor)
{
    std::string node_name(node->mName.C_Str());
    glm::mat4 node_transformation = Math::AssimpToGlmMatrix(node->mTransformation);

    LocalTransform start_transform;
    LocalTransform end_transform;

    const aiNodeAnim *start_node_anim = FindNodeAnim(start_animation, node_name);
    if (start_node_anim)
    {
        CalcLocalTransform(start_transform, start_anim_time_ticks, start_node_anim);
    }

    const aiNodeAnim *end_node_anim = FindNodeAnim(end_animation, node_name);
    if (!start_node_anim || !end_animation)
    {
        OGN_CORE_ERROR("On the node {} ther is an animation node for only one of the start/end animations.\nIn this case is not supported", node_name);
    }

    if (end_node_anim)
    {
        CalcLocalTransform(end_transform, end_anim_time_ticks, end_node_anim);
    }

    if (start_node_anim && end_node_anim)
    {
        // interpolate scaling
        const aiVector3D &scale_0 = start_transform.scaling;
        const aiVector3D &scale_1 = end_transform.scaling;
        const aiVector3D blended_scaling = (1.0f - blend_factor) * scale_0 + scale_1 * blend_factor;
        const glm::mat4 scaling_transform = glm::scale(glm::mat4(1.0f), { blended_scaling.x, blended_scaling.y , blended_scaling.z });

        // interpolate rotation
        const aiQuaternion &rot_0 = start_transform.rotation;
        const aiQuaternion &rot_1 = end_transform.rotation;
        aiQuaternion blended_rotation;
        aiQuaternion::Interpolate(blended_rotation, rot_0, rot_1, blend_factor);
        const glm::quat rotation_transform = Math::AssimpToGlmQuat(blended_rotation);

        // interpolate translation
        const aiVector3D &pos_0 = start_transform.translation;
        const aiVector3D &pos_1 = end_transform.translation;
        const aiVector3D blended_translation = (1.0f - blend_factor) * pos_0 + pos_1 * blend_factor;
        const glm::mat4 translation_transform = glm::translate(glm::mat4(1.0f), { blended_translation.x, blended_translation.y, blended_translation.z });

        node_transformation = translation_transform * glm::toMat4(rotation_transform) * scaling_transform;
    }

    const glm::mat4 global_transformation = parent_transform * node_transformation;

    if (m_BoneNameToIndexMap.contains(node_name))
    {
        const u32 bone_index = m_BoneNameToIndexMap[node_name];
        m_BoneInfo[bone_index].final_transformation = m_GlobalInverseTransform * global_transformation * m_BoneInfo[bone_index].offset_matrix;
    }

    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        std::string child_name(node->mChildren[i]->mName.C_Str());
        auto it = m_RequiredNodeMap.find(child_name);
        if (it == m_RequiredNodeMap.end())
        {
            OGN_CORE_ASSERT(false, "Child {} cannot be found in the required node map", child_name);
        }

        if (it->second.is_requiered)
        {
            ReadNodeHierarchyBlended(start_anim_time_ticks, end_anim_time_ticks, node->mChildren[i], global_transformation,
                start_animation, end_animation, blend_factor);
        }
    }
}

void SkinnedMesh::InitializeRequiredNodeMap(const aiNode *node)
{
    std::string node_name(node->mName.C_Str());
    NodeInfoA info(node);

    m_RequiredNodeMap[node_name] = info;
    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        InitializeRequiredNodeMap(node->mChildren[i]);
    }
}

void SkinnedMesh::MarkRequiredNodesForBone(const aiBone *bone)
{
    std::string node_name(bone->mName.C_Str());
    const aiNode *parent = nullptr;
    do
    {
        auto it = m_RequiredNodeMap.find(node_name);
        if (it == m_RequiredNodeMap.end())
        {
            OGN_CORE_ASSERT(false, "Cannot find bone {} is the hierarchy", node_name);
        }

        it->second.is_requiered = true;

        parent = it->second.node->mParent;

        if (parent)
        {
            node_name = std::string(parent->mName.C_Str());
        }

    } while (parent);
}

f32 SkinnedMesh::CalcAnimationTimeTicks(f32 time_in_seconds, u32 anim_index)
{
    f32 ticks_persecond = static_cast<float>(m_Scene->mAnimations[anim_index]->mTicksPerSecond != 0 ? m_Scene->mAnimations[anim_index]->mTicksPerSecond : 25.0f);
    f32 time_in_ticks = time_in_seconds * ticks_persecond;
    f32 duration = 0.0f;
    f32 fraction = modf(static_cast<f32>(m_Scene->mAnimations[anim_index]->mDuration), &duration);
    return fmod(time_in_ticks, duration);
}

void SkinnedMesh::CalcLocalTransform(LocalTransform &transform, f32 anim_time_ticks, const aiNodeAnim *node_anim)
{
    CalcInterpolatedScaling(transform.scaling, anim_time_ticks, node_anim);
    CalcInterpolatedRotation(transform.rotation, anim_time_ticks, node_anim);
    CalcInterpolatedPosition(transform.translation, anim_time_ticks, node_anim);
}

bool SkinnedMesh::InitMaterials(const aiScene *scene, const std::string &filename)
{
    return true;
}

void SkinnedMesh::LoadTextures(const std::string &dir, const aiMaterial *material, i32 index)
{

}

void SkinnedMesh::LoadDiffuseTexture(const std::string &dir, const aiMaterial *material, i32 index)
{

}

void SkinnedMesh::LoadSpecularTexture(const std::string &dir, const aiMaterial *material, i32 index)
{

}

void SkinnedMesh::LoadColors(const aiMaterial *material, i32 index)
{

}

