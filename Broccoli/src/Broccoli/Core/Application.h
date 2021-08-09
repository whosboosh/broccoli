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

		Ref<Mesh> getTestMesh() { return mesh; }

		inline Camera& getCamera() { return *camera; }
		inline Window& getWindow() { return *window; }
		inline Renderer& getRenderer() { return *renderer; }

		void setMinimise(bool state) { isMinimised = state; }

	private:
		Ref<Mesh> mesh;
		Ref<Model> model;


		ViewProjection viewProjection;
		
		Camera* camera;
		Window* window;
		Renderer* renderer;
		ApplicationInfo appInfo;
		static Application* appInstance;
		bool isRunning = true, isMinimised = false;
		uint64_t frameCounter = 0;
		float lastFrameTime = 0, deltaTime = 0, currentFpsTime = 0;
	};

	//Application* CreateApplication(int argc, char** argvv);

}