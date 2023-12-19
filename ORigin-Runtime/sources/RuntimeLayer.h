#pragma once
#include <Origin.h>

class RuntimeLayer : public origin::Layer
{
public:
	RuntimeLayer();
	~RuntimeLayer();

	void OnAttach() override;
	void OnUpdate(origin::Timestep ts) override;

	bool OpenProject(const std::filesystem::path& path);
	void OpenScene(origin::AssetHandle handle);

private:
	std::shared_ptr<origin::Scene> m_ActiveScene;
};

