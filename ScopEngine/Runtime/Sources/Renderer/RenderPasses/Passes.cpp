#include <Renderer/RenderPasses/Passes.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>

namespace Scop
{
	void RenderPasses::Init()
	{
		m_skybox.Init();
		m_final.Init();
	}

	void RenderPasses::Pass(Scene& scene, Renderer& renderer)
	{
		if(!m_main_render_texture.IsInit())
		{
			std::function<void(const EventBase&)> functor = [this, renderer](const EventBase& event)
			{
				if(event.What() == 56)
				{
					m_main_render_texture.Destroy();
					auto extent = kvfGetSwapchainImagesSize(renderer.GetSwapchain());
					m_main_render_texture.Init({}, extent.width, extent.height);
				}
			};
			EventBus::RegisterListener({ functor, "__ScopRenderPasses" });
			auto extent = kvfGetSwapchainImagesSize(renderer.GetSwapchain());

			m_main_render_texture.Init({}, extent.width, extent.height);
		}
		m_forward.Pass(scene, renderer, m_main_render_texture);
		m_skybox.Pass(scene, renderer, m_main_render_texture);

		m_final.Pass(scene, renderer, m_main_render_texture);
	}

	void RenderPasses::Destroy()
	{
		m_skybox.Destroy();
		m_final.Destroy();
		m_main_render_texture.Destroy();
	}
}
