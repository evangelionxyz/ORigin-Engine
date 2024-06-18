#pragma once
#include <Origin.h>

#ifdef APIENTRY
	#undef APIENTRY
#endif

class RuntimeLayer : public origin::Layer
{
public:
	RuntimeLayer()
		: Layer("Runtime Layer")
	{
	}

	void OnAttach() override;
	void OnUpdate(origin::Timestep ts) override;
	void OnEvent(origin::Event &e) override;
	void OnLoadingScreen(uint32_t waitingTime);

	bool OpenProject(const std::filesystem::path &path);
	void OpenScene(origin::AssetHandle handle);

	bool OnResize(origin::FramebufferResizeEvent &e);
	bool OnKeyPressed(origin::KeyPressedEvent &e);
	std::shared_ptr<origin::Texture2D> m_LoadingScreenImage;
	std::shared_ptr<origin::Scene> m_ActiveScene;
	bool IsLoaded = false;
};

