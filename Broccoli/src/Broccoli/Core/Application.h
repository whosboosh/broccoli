#include "Window.h"
#include "Camera.h"

#include "Broccoli/Utilities/VulkanUtilities.h"

#include "Broccoli/Renderer/Renderer.h"

namespace Broccoli {
	struct ApplicationInfo
	{
		std::string Name = "Broccoli";
		uint32_t windowWidth = 1600, windowHeight = 1600;
		bool fullscreen = false;
		bool vsync = false;
		float farPlane = 200.0f;
		float nearPlane = 0.1f;
	};

	class Application
	{
	public:
		Application(int argc, char** argvv);

		void Run();
		void Close();

		void OnInit();
		void Shutdown();

		void updateUniforms();
		void processEvents();

		const ApplicationInfo& getSpecification() const { return appInfo; }
		static inline Application& get() { return *appInstance; }

		inline Camera& getCamera() { return *camera; }
		inline Window& getWindow() { return *window; }
		inline Renderer& getRenderer() { return *renderer; }

		void setMinimise(bool state) { isMinimised = state; }

	private:
		std::vector<Ref<Mesh>> meshList;
		std::vector<Ref<Model>> modelList;


		ViewProjection viewProjection;
		
		Camera* camera;
		Window* window;
		Renderer* renderer;
		ApplicationInfo appInfo;
		static Application* appInstance;
		bool isMinimised = false;
		uint64_t frameCounter = 0;
		float lastFrameTime = 0, deltaTime = 0, currentFpsTime = 0;
	};

	//Application* CreateApplication(int argc, char** argvv);

}