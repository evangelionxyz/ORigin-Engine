// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Mesh.hpp"
#include "Origin/Renderer/Materials/Material.hpp"
#include "Origin/Animation/Animation.h"
#include "Origin/Animation/AnimationBlender.h"

#include <filesystem>

namespace origin {

struct BoneInfo
{
    std::string name;
    glm::mat4 offset_matrix;
    glm::mat4 transform;

    BoneInfo(const glm::mat4 &offset)
    {
        offset_matrix = offset;
        transform = glm::mat4(1.0f);
    }
};

struct NodeInfo
{
    NodeInfo() = default;
    NodeInfo(const aiNode *node)
        : node(node)
    {
    }

    const aiNode *node = nullptr;
    bool is_required = false;
};

class Model : public Asset
{
public:
	Model() = default;
	Model(const std::string &filepath);

    void ProcessNode(aiNode *node, const glm::mat4 &trasnform, const std::string &filepath);

	void UpdateAnimation(f32 delta_time, const u32 anim_index = 0);
	void LoadSingleMesh(const u32 mesh_index, aiMesh *mesh, const glm::mat4 &transform, const std::string &filepath);
    void LoadAnimations();
	void LoadVertexBones(const u32 mesh_index, Ref<Mesh> &data, aiMesh *mesh);
	void LoadSingleVertexBone(const u32 mesh_index, Ref<Mesh> &data, const aiBone *bone);
	void LoadMaterials(MeshMaterial &material, aiMaterial *ai_material, const std::string &filepath);
    void CalculateBoneTransforms(const aiNode *node, const glm::mat4 &parent_transform, const u32 anim_index);
    void CalculateAnimationTransforms(const aiNode *node, const u32 anim_index, std::unordered_map<std::string, AnimationNode> &anim_nodes, const glm::mat4 &parent_transform = glm::mat4(1.0f));

	Ref<Texture2D> LoadTexture(const aiScene *scene, aiMaterial *material, const std::string &filepath, TextureType type);
	void CreateVertex(Ref<Mesh> &mesh_data);
    std::vector<SkeletalAnimation> &GetAnimations();
    SkeletalAnimation *GetAnimation(u32 index);
    const std::vector<Ref<Mesh>> &GetMeshes();
	i32 GetBoneID(const aiBone *bone);
    const bool HasAnimations() const;
    const std::vector<glm::mat4> &GetBoneTransforms() const;

	static Ref<Model> Create(const std::string &filepath);
    static AssetType GetStaticType() { return AssetType::Mesh; }
    AssetType GetType() const override { return GetStaticType(); }

private:
	Assimp::Importer m_Importer;
	const aiScene *m_Scene = nullptr;
	
	std::vector<BoneInfo> m_BoneInfo;
    std::vector<glm::mat4> m_FinalBoneTransforms;
	std::vector<Ref<Mesh>> m_Meshes;
	std::vector<SkeletalAnimation> m_Animations;
	std::unordered_map<std::string, u32> m_BoneNameToIndexMap;

    friend class AnimationBlender;
};


}

#endif