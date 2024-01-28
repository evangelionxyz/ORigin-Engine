#pragma once
#include <Origin.h>

class RuntimeLayer : public origin::Layer
{
public:
	RuntimeLayer()
		: Layer("Runtime Layer")
	{
	}

	void OnAttach() override;
	void OnUpdate(origin::Timestep ts) override;
	void OnEvent(origin::Event& e);

	bool OpenProject(const std::filesystem::path& path);
	void OpenScene(origin::AssetHandle handle);

	bool OnResize(origin::FramebufferResizeEvent& e);

private:
	std::shared_ptr<origin::Scene> m_ActiveScene;
};

