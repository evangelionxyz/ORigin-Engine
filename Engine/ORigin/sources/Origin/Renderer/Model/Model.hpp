// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Mesh.hpp"
#include "MeshMaterial.hpp"
#include "Bone.hpp"

#include <filesystem>

namespace origin {

class Anim
{
public:
	Anim() = default;
    Anim(aiAnimation *anim)
        : m_Anim(anim)
    {
        m_Name = anim->mName.C_Str();
        m_Duration = static_cast<float>(anim->mDuration);
        m_TicksPerSecond = static_cast<float>(anim->mTicksPerSecond);
        if (anim->mTicksPerSecond == 0.0) m_TicksPerSecond = 1.0f;

        ReadChannels(anim);
    }

    void UpdateTime(const f32 delta_time)
    {
        m_TimeInSeconds += delta_time;
        m_TimeInTicks = m_TimeInSeconds * m_TicksPerSecond;
    }

    const std::string &GetName() const { return m_Name; }
    const f32 GetDuration() const { return m_Duration; }
    const f32 GetTicksPerSecond() const { return m_TicksPerSecond; }
    const f32 GetTimeInSecons() const { return m_TimeInSeconds; }
    const f32 GetTimeInTicks() const { return m_TimeInTicks; }

    std::unordered_map<std::string, AnimationNode> &GetChannelMap() { return m_ChannelMap; }

private:
    void ReadChannels(const aiAnimation *anim)
    {
        for (u32 i = 0; i < anim->mNumChannels; ++i)
        {
            aiNodeAnim *node_anim = anim->mChannels[i];
            std::string node_name(node_anim->mNodeName.C_Str());
            m_ChannelMap[node_name] = AnimationNode(node_anim);
        }
    }

    std::string m_Name;
    aiAnimation *m_Anim = nullptr;
    f32 m_Duration = 0.0f;
    f32 m_TicksPerSecond = 1.0f;
    f32 m_TimeInSeconds = 0.0f;
    f32 m_TimeInTicks = 0.0f;
    std::unordered_map<std::string, AnimationNode> m_ChannelMap;
};

class Model : public Asset
{
public:
	Model() = default;
	Model(const std::string &filepath);

	void UpdateAnimation(f32 delta_time, const u32 anim_index = 0);
    const std::vector<glm::mat4> &GetBoneTransforms() const;

	void LoadMeshes(const aiScene *scene, const std::string &filepath);
	void LoadSingleMesh(const u32 mesh_index, aiMesh *mesh, const std::string &filepath);
    void LoadAnimations();

    void CalculateBoneTransforms(f32 time_in_ticks, const aiNode *node, const glm::mat4 &parent_transform, const u32 anim_index);

	void LoadVertexBones(const u32 mesh_index, Ref<Mesh> &data, aiMesh *mesh);
	void LoadSingleVertexBone(const u32 mesh_index, Ref<Mesh> &data, const aiBone *bone);
	void LoadMaterials(Ref<Mesh> mesh_data, aiMesh *mesh, const std::string &filepath);
	
	TextureTypeMap LoadTextures(const aiScene *scene, aiMaterial *material, const std::string &filepath, TextureType type);
	void CreateVertex(Ref<Mesh> &mesh_data);
    const std::vector<Anim> &GetAnimations();
    const std::vector<Ref<Mesh>> &GetMeshes();
	i32 GetBoneID(const aiBone *bone);
    const bool HasAnimations() const;

	static Ref<Model> Create(const std::string &filepath);
    static AssetType GetStaticType() { return AssetType::Mesh; }
    virtual AssetType GetType() const { return GetStaticType(); }

private:
	Assimp::Importer m_Importer;
	const aiScene *m_Scene = nullptr;
	
	std::vector<BoneInfo> m_BoneInfo;
	std::vector<VertexBoneData> m_GlobalBones;
	std::unordered_map<std::string, u32> m_BoneNameToIndexMap;

	std::vector<Ref<Mesh>> m_Meshes;
	std::vector<Anim> m_Animations;

    std::vector<glm::mat4> m_FinalBoneTransforms;
};

}

#endif