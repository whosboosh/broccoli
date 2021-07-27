#include "Window.h"

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

		const ApplicationInfo& getSpecification() const { return appInfo; }
		static inline Application& get() { return *appInstance; }

		inline Window& getWindow() { return *window; }
		inline Renderer& getRenderer() { return *renderer; }

	private:
		Ref<Mesh> mesh;

		Window* window;
		Renderer* renderer;
		ApplicationInfo appInfo;
		static Application* appInstance;
		bool isRunning = true, isMinimised = false;
		uint64_t frameCounter = 0, lastFrameTime = 0, deltaTime = 0;
	};

	//Application* CreateApplication(int argc, char** argvv);

}