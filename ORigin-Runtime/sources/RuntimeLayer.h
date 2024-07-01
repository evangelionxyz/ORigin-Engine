#pragma once
#include <Origin.h>

#ifdef APIENTRY
	#undef APIENTRY
#endif

class RuntimeLayer : public origin::Layer
{
public:
	RuntimeLayer();
	~RuntimeLayer();

	void OnAttach() override;
	void OnUpdate(origin::Timestep ts) override;
	void OnEvent(origin::Event &e) override;
	void OnLoadingScreen(float endTime);
	bool OnWindowResized(origin::WindowResizeEvent &e);
	bool OnKeyPressed(origin::KeyPressedEvent &e);
	void Resize(uint32_t width, uint32_t height);
	bool OpenProject(const std::filesystem::path &path);

	std::shared_ptr<origin::AudioSource> m_LoadingSound;
	std::shared_ptr<origin::Texture2D> m_ScreenTexture;
	std::shared_ptr<origin::Scene> m_ActiveScene;

	float X, Y;
	float Width, Height;
};

