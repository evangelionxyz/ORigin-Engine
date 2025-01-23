// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Mesh.hpp"
#include "MeshMaterial.hpp"
#include "Origin/Animation/ModelAnimation.hpp"

#include <filesystem>

namespace origin {

class Anim
{
public:
	Anim() = default;
	Anim(const aiAnimation *anim, std::map<std::string, Bone> &global_bones)
		: duration(static_cast<f32>(anim->mDuration)),
		ticks_per_second(static_cast<f32>(anim->mTicksPerSecond ? anim->mTicksPerSecond : 25.0f))
	{
		for (u32 i = 0; i < anim->mNumChannels; ++i)
		{
			const aiNodeAnim *channel = anim->mChannels[i];
			std::string bone_name(channel->mNodeName.C_Str());

			if (global_bones.contains(bone_name))
			{
				bone_animation_map[bone_name] = channel;
			}
		}

	}

	const aiNodeAnim *FindNodeAnim(const std::string &bone_name) const
	{
		auto it = bone_animation_map.find(bone_name);
		return it != bone_animation_map.end() ? it->second : nullptr;
	}

	f32 duration;
	f32 ticks_per_second;
	std::map<std::string, const aiNodeAnim *> bone_animation_map;
};

class Model : public Asset
{
public:
	Model() = default;
	Model(const std::string &filepath);

	void LoadBones(Ref<Mesh> &data, aiMesh *mesh);
	TextureTypeMap LoadTextures(const aiScene *scene, aiMaterial *material, const std::string &filepath, TextureType type);
	Ref<Mesh> LoadMeshData(const aiScene *scene, aiMesh *mesh, const std::string &filepath);
	void LoadMeshes(const aiScene *scene, const std::string &filepath);
	aiNode *FindMeshNode(aiNode *node, const aiScene *scene, aiMesh *target_mesh);
	void CreateVertex(Ref<Mesh> &mesh_data);
    const std::vector<Anim> &GetAnimations();
    const std::vector<Ref<Mesh>> &GetMeshes();

	static Ref<Model> Create(const std::string &filepath);
    static AssetType GetStaticType() { return AssetType::Mesh; }
    virtual AssetType GetType() const { return GetStaticType(); }

	void LoadAnimations(const aiScene *scene);
	void GetFinalBoneTransforms(std::vector<glm::mat4> &out_bone_transforms);
	void UpdateAnimation(const f32 dt, const f32 speed = 1.0f);
	void BuildBoneHierarchy(const aiNode *node, std::map<std::string, Bone> &global_bones);

	Anim *current_animation = nullptr;
	f32 current_time = 0.0f;

private:
	Assimp::Importer m_Importer;
	const aiScene *m_Scene = nullptr;
	std::map<std::string, Bone> global_bones;
	std::vector<Ref<Mesh>> m_Meshes;
	std::vector<Anim> m_Animations;
};

}

#endif