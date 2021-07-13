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

		VkSwapchainKHR oldSwapchain = swapChain;

		// Get details about the swapchain based on surface and physical device for best settings
		SwapChainDetails swapChainDetails = getSwapchainDetails(physicalDevice);

		// Find optimal surface values for our swapchain
		VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapChainDetails.formats);
		VkPresentModeKHR presentMode = chooseBestPresentationMode(swapChainDetails.presentationMode);
		swapChainExtent = chooseSwapExtent(swapChainDetails.surfaceCapabilities, width, height);

		// How many images are in the swapchain? Get 1 more than minimum to allow triple buffering
		uint32_t imageCount = swapChainDetails.surfaceCapabilities.minImageCount + 1;
		// Check that we haven't gone over maximum image count, if 0 then limitless
		if (swapChainDetails.surfaceCapabilities.maxImageCount > 0 && swapChainDetails.surfaceCapabilities.maxImageCount < imageCount)
		{
			imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
		}


		// Creation information for swap chain
		VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
		swapChainCreateInfo.surface = surface;
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.imageFormat = surfaceFormat.format;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.imageExtent = swapChainExtent;
		swapChainCreateInfo.minImageCount = imageCount; // Number of images in swap chain (min)
		swapChainCreateInfo.imageArrayLayers = 1; // Number of layers for each image in chain
		swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // What attachment images will be used as
		swapChainCreateInfo.preTransform = swapChainDetails.surfaceCapabilities.currentTransform; // Transform to perform on swap chain images
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // How to handle blending images with external graphics (e.g. other windows)
		swapChainCreateInfo.clipped = VK_TRUE; // Wheher to clip part of images not in view
		swapChainCreateInfo.oldSwapchain = oldSwapchain;

		// TODO: after staging buffers used see if this is needed
		// Enable transfer source on swap chain images if supported
		if (swapChainDetails.surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
			swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		// Enable transfer destination on swap chain images if supported
		if (swapChainDetails.surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
			swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		// Create swapchain
		VkResult result = vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create swapchain");
		}
		else {
			std::cout << "Created the vulkan swapchain!\n";
		}

		// Recreation of the swapchain, cleanup the previous images.
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			for (uint32_t i = 0; i < swapChainImageCount; i++)
			{
				vkDestroyImageView(device, swapChainImages[i].imageView, nullptr);
			}
			vkDestroySwapchainKHR(device, oldSwapchain, nullptr);
		}

		// Get the swapchain images
		vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, nullptr);
		std::vector<VkImage> images(swapChainImageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, images.data());

		for (VkImage image : images) {

			// Store the image handle
			SwapChainImage swapChainImage = {};
			swapChainImage.image = image;
			swapChainImage.imageView = createImageView(image, surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);

			// Add image to swapChainImages List in global namespace
			swapChainImages.push_back(swapChainImage);
		}
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
		// TODO: Vsync support
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

	VkImageView VulkanSwapchain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewCreateInfo = {};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = image; // Image to create view for
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // Type of image (1D, 2D, 3D, Cube etc)
		viewCreateInfo.format = format; // Format of image data
		viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // Allows remapping of rgba components to other rgba values
		viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// Subresources allow the view to view only a part of an images
		viewCreateInfo.subresourceRange.aspectMask = aspectFlags; // Which aspect of image to view (e.g. COLOR_BIT for viewing color)
		viewCreateInfo.subresourceRange.baseMipLevel = 0; // Start mipmap level to view from
		viewCreateInfo.subresourceRange.levelCount = 1; // Number of mipmap levels to view
		viewCreateInfo.subresourceRange.baseArrayLayer = 0; // Start array level to view from
		viewCreateInfo.subresourceRange.layerCount = 1; // How many array levels to view

		// Create image view and return it
		VkImageView imageView;
		VkResult result = vkCreateImageView(logicalDevice->getLogicalDevice(), &viewCreateInfo, nullptr, &imageView);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create an image view");
		}
		else {
			std::cout << "Created image view\n";
		}

		return imageView;
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
	}
}
