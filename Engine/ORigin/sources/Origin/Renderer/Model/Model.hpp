// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Mesh.hpp"
#include "MeshMaterial.hpp"
#include "Origin/Animation/Animation.h"
#include "Origin/Animation/AnimationBlender.h"

#include <filesystem>

namespace origin {

struct BoneInfo
{
    glm::mat4 offset_matrix;
    glm::mat4 final_transformation;

    BoneInfo(const glm::mat4 &offset)
    {
        offset_matrix = offset;
        final_transformation = glm::mat4(1.0f);
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

	void UpdateAnimation(f32 delta_time, const u32 anim_index = 0);
	void LoadMeshes(const aiScene *scene, const std::string &filepath);
	void LoadSingleMesh(const u32 mesh_index, aiMesh *mesh, const std::string &filepath);
    void LoadAnimations();
	void LoadVertexBones(const u32 mesh_index, Ref<Mesh> &data, aiMesh *mesh);
	void LoadSingleVertexBone(const u32 mesh_index, Ref<Mesh> &data, const aiBone *bone);
	void LoadMaterials(Ref<Mesh> mesh_data, aiMesh *mesh, const std::string &filepath);
    void CalculateBoneTransforms(f32 time_in_ticks, const aiNode *node, const glm::mat4 &parent_transform, const u32 anim_index);
	
	TextureTypeMap LoadTextures(const aiScene *scene, aiMaterial *material, const std::string &filepath, TextureType type);
	void CreateVertex(Ref<Mesh> &mesh_data);
    std::vector<SkeletalAnimation> &GetAnimations();
    SkeletalAnimation *GetAnimation(u32 index);
    const std::vector<Ref<Mesh>> &GetMeshes();
	i32 GetBoneID(const aiBone *bone);
    const bool HasAnimations() const;
    const std::vector<glm::mat4> &GetBoneTransforms() const;

	static Ref<Model> Create(const std::string &filepath);
    static AssetType GetStaticType() { return AssetType::Mesh; }
    virtual AssetType GetType() const { return GetStaticType(); }

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