#include "VulkanContext.h"

#include "VulkanValidation.h"

#include <GLFW/glfw3.h>

namespace Broccoli {

	VulkanContext::VulkanContext()
	{
	}
	
	VulkanContext::~VulkanContext()
	{
		logicalDevice->cleanup();

		vkDestroyInstance(mainInstance, nullptr);
		mainInstance = nullptr;

		vkDestroySurfaceKHR(mainInstance, surface, nullptr);
		// Cleanup validation
		if (enableValidationLayers)
		{
			destroyDebugMessenger(nullptr);
		}
	}

	void VulkanContext::init(GLFWwindow* windowHandle)
	{
		std::cout << "Vulkan Context Creation starting\n";

		// Setup Vulkan
		if (!glfwVulkanSupported())
		{
			std::cout << "GLFW: Vulkan Not Supported\n";
			return;
		}

		// Create the vulkan instance and validation layers
		// APP INFO
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Cloud Gaming Engine";
		appInfo.pEngineName = "Broccoli";
		appInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = nullptr;
		instanceCreateInfo.pApplicationInfo = &appInfo;

		// Debug Utils for instance error checking (before debug utils has been setup)
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

		// Include the validation layer names if they are enabled
		if (enableValidationLayers) {
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			instanceCreateInfo.enabledLayerCount = 0;
			instanceCreateInfo.pNext = nullptr;
		}

		auto instanceExtensions = getRequiredExtensions();
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

		// Check instance extensions supported
		if (!checkInstanceExtensionSupport(&instanceExtensions))
		{
			throw std::runtime_error("VkInstance does not support required extensions");
		}

		// Check Validation Layers are supported
		if (enableValidationLayers && !checkValidiationLayerSupport()) {
			throw std::runtime_error("Validation layers requested, but not available");
		}

		// Create instance
		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &mainInstance);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create a Vulkan Instance");
		}

		// Call to VulkanValidation, create the debug messenger for debug callbacks
		setupDebugMessenger();
		std::cout << "Vulkan validation layers created...\n";


		// Create the surface (Creating a surface create info struct, specific to GLFW window type) - returns result
		result = glfwCreateWindowSurface(VulkanContext::getInstance(), windowHandle, nullptr, &surface);
		if (result != VK_SUCCESS) throw std::runtime_error("Failed to create surface");
		std::cout << "Vulkan surface creation successful\n";

		// Create the vulkan devices (logical and physical)
		physicalDevice = Ref<VulkanPhysicalDevice>::create();
		logicalDevice = Ref<VulkanLogicalDevice>::create(physicalDevice);
	}

}
