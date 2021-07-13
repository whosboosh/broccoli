#include "VulkanSwapchain.h"

namespace Broccoli {
	VulkanSwapchain::VulkanSwapchain()
	{
	}

	void VulkanSwapchain::init(VkInstance instance, const Ref<VulkanLogicalDevice>& logicalDevice, VkSurfaceKHR surface)
	{
		this->instance = instance;
		this->logicalDevice = logicalDevice;
		this->surface = surface;
	}

	void VulkanSwapchain::create(uint32_t* width, uint32_t* height, bool vsync)
	{
		this->vsync = vsync;

		VkDevice device = logicalDevice->getLogicalDevice();
		VkPhysicalDevice physicalDevice = logicalDevice->getPhysicalDevice()->getVulkanPhysicalDevice();

		// Get details about the swapchain based on surface and physical device for best settings
		SwapChainDetails swapChainDetails = getSwapchainDetails(physicalDevice);

		// Find optimal surface values for our swapchain
		VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapChainDetails.formats);
		VkPresentModeKHR presentMode = chooseBestPresentationMode(swapChainDetails.presentationMode);
		swapChainExtent = chooseSwapExtent(swapChainDetails.surfaceCapabilities, width, height);

	}

	SwapChainDetails VulkanSwapchain::getSwapchainDetails(VkPhysicalDevice physicalDevice)
	{
		SwapChainDetails swapChainDetails;

		// Get the surface capabiltiies of given surface for given physical device
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapChainDetails.surfaceCapabilities);

		// Formats
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			swapChainDetails.formats.resize(formatCount); // Resize vector to the supported formats for given surface
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapChainDetails.formats.data());
		}

		// Presentation Modes
		uint32_t presentationCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationCount, nullptr);

		if (presentationCount != 0)
		{
			swapChainDetails.presentationMode.resize(presentationCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationCount, swapChainDetails.presentationMode.data());
		}


		return swapChainDetails;
	}

	// Best format various based on application specification, will use VK_FORMAT_R8G8B8A8_UNIFORM || VK_FORMAT_B8G8R8A8_UNORM (Backup)
	// Color space, SRGB, AdobeRGB etc. Will use VK_COLOR_SPACE_SRGB_NONLINEAR_KHR (SRGB)
	VkSurfaceFormatKHR VulkanSwapchain::chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) // If all formats are supported (VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_R8G8B8A8_UNORM , VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		// If restricted, search for optimal format
		for (const auto& format : formats)
		{
			// if RGB OR BGR are available
			if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || VK_FORMAT_B8G8R8A8_UNORM) && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}
		// If can't find optimal format, then just return first format
		return formats[0];
	}

	VkPresentModeKHR VulkanSwapchain::chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes)
	{
		for (const auto& presentationMode : presentationModes)
		{
			if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return presentationMode;
			}
		}

		// If mailbox doesn't exist, use FIFO as Vulkan spec requires it to be present
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, uint32_t* width, uint32_t* height)
	{
		if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) // If current extend is at numeric limits, the extend van vary. Otherwise it is the size of the window.
		{
			*width = surfaceCapabilities.currentExtent.width;
			*height = surfaceCapabilities.currentExtent.height;
			return surfaceCapabilities.currentExtent;
		}
		else {
			// If value can vary, need to set manually
			// Create new extent using window size
			VkExtent2D newExtent = {};
			newExtent.width = *width;
			newExtent.height = *height;

			// Surface also defines max and min, make sure inside boundaries by clamping value
			newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width)); // Keeps width within the boundaries
			newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));

			return newExtent;
		}
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
	}
}
