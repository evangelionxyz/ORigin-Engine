#pragma once
#include "Mesh.h"
#include "Origin/Animation/ModelAnimation.h"
#include <filesystem>

namespace origin {

struct ModelData
{
	aiScene *ai_scene;
	aiNode *ai_root_node;
};

class Model : public Asset
{
public:
	Model() = default;
	Model(const std::string &filepath);

	const std::vector<ModelAnimation> &GetAnimations();
	const std::vector<Ref<Mesh>> &GetMeshes();
	const std::string &GetFilepath();

	static Ref<Model> Create(const std::string &filepath);
	static Ref<Mesh> LoadMeshData(const aiScene *scene, aiMesh *mesh, const std::string &filepath);
	static const aiScene *LoadAiScene(const std::string &filepath);
	static std::vector<Ref<Mesh>> ProcessNode(aiNode *node, const aiScene *scene, const std::string &filepath);
	static aiNode *FindMeshNode(aiNode *node, const aiScene *scene, aiMesh *target_mesh);
	static glm::mat4 CalculateTransform(aiNode *node, aiMesh *mesh);

	static std::vector<ModelAnimation> LoadAnimations(const std::vector<Ref<Mesh>> &meshes, const aiScene *scene);
	static void CreateVertex(const Ref<Mesh> &mesh_data);

    static AssetType GetStaticType() { return AssetType::Mesh; }
    virtual AssetType GetType() const { return GetStaticType(); }

private:
	std::vector<Ref<Mesh>> m_Meshes;
	std::vector<ModelAnimation> m_Animations;
	std::string m_Filepath;
};

}
