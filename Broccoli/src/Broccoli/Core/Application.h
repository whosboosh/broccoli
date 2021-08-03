#include "Window.h"
#include "Camera.h"

#include "Broccoli/Renderer/Renderer.h"

namespace Broccoli {
	struct ApplicationInfo
	{
		std::string Name = "Broccoli";
		uint32_t windowWidth = 1600, windowHeight = 1600;
		bool fullscreen = false;
		bool vsync = false;
	};

	struct ViewProjection {
		glm::mat4 projection;
		glm::mat4 view;
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

		inline Window& getWindow() { return *window; }
		inline Renderer& getRenderer() { return *renderer; }

		void setMinimise(bool state) { isMinimised = state; }

	private:
		Ref<Mesh> mesh;
		ViewProjection viewProjection;
		
		Camera* camera;
		Window* window;
		Renderer* renderer;
		ApplicationInfo appInfo;
		static Application* appInstance;
		bool isRunning = true, isMinimised = false;
		uint64_t frameCounter = 0, lastFrameTime = 0, deltaTime = 0;
	};

	//Application* CreateApplication(int argc, char** argvv);

}