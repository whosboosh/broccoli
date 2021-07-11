#pragma once

namespace Broccoli {
	
	enum class RendererAPIType
	{
		None,
		Vulkan,
		OpenGL
	};

	enum class PrimitiveType
	{
		None = 0, Triangles, Lines
	};

	class RendererAPI
	{
	public:
		virtual void init();
		virtual void shutdown();

		virtual void beginFrame();
		virtual void endFrame();

		virtual void beginRenderPass();
		virtual void endRenderPass();

		// Render mesh etc...
		static RendererAPIType getCurrent() { return currentRenderer; }

	private:
		// TODO: Don't make this const, use inline?
		const static RendererAPIType currentRenderer = RendererAPIType::Vulkan;
	};

}