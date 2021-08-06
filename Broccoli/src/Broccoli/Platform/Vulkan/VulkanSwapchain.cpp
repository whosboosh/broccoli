#include "VulkanSwapchain.h"

#include "Broccoli/Core/Application.h"

#include "Broccoli/Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>

namespace Broccoli {
	VulkanSwapchain::VulkanSwapchain()
	{
	}

	void VulkanSwapchain::init(VkInstance instance, VulkanLogicalDevice* logicalDevice, bool vsync)
	{
		this->instance = instance;
		this->logicalDevice = logicalDevice;
		this->vsync = vsync;

		// Create synchronisation objects (semaphores & fences)
		createSynchronisation();

		create(vsync);
	}

	void VulkanSwapchain::create(bool vsync)
	{
		//VkSwapchainKHR oldSwapchain = swapChain;

		// Get details about the swapchain based on surface and physical device for best settings
		SwapChainDetails swapChainDetails = getSwapchainDetails(getPhysicalDevice(), VulkanContext::get()->getSurface());

		// Find optimal surface values for our swapchain
		surfaceFormat = chooseBestSurfaceFormat(swapChainDetails.formats);
		VkPresentModeKHR presentMode = chooseBestPresentationMode(swapChainDetails.presentationMode);
		swapChainExtent = chooseSwapExtent(swapChainDetails.surfaceCapabilities);

		// How many images are in the swapchain? Get 1 more than minimum to allow triple buffering
		uint32_t imageCount = swapChainDetails.surfaceCapabilities.minImageCount + 1;
		// Check that we haven't gone over maximum image count, if 0 then limitless
		if (swapChainDetails.surfaceCapabilities.maxImageCount > 0 && swapChainDetails.surfaceCapabilities.maxImageCount < imageCount)
		{
			imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
		}

		// Create depth stencil for depth buffering
		createDepthStencil();

		// Creation information for swap chain
		VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
		swapChainCreateInfo.surface = VulkanContext::get()->getSurface();
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
		swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;


		// Get Queue Family Indicies
		QueueFamilyIndicies indicies = logicalDevice->getPhysicalDevice()->getQueueFamilyIndicies();

		// If Graphics and Presentation are different, then swapchain must let images be shared between families
		if (indicies.graphicsFamily != indicies.presentationFamily) {

			uint32_t queueFamilyIndicies[] = {
				(uint32_t)indicies.graphicsFamily,
				(uint32_t)indicies.presentationFamily
			};

			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Image share handling
			swapChainCreateInfo.queueFamilyIndexCount = 2; // Number of queues to share images between
			swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndicies; // Array of queues to share between
		}
		else {
			// Only 1 queue family
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapChainCreateInfo.queueFamilyIndexCount = 0;
			swapChainCreateInfo.pQueueFamilyIndices = nullptr;
		}

		/*
		// TODO: after staging buffers used see if this is needed
		// Enable transfer source on swap chain images if supported
		if (swapChainDetails.surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
			swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		// Enable transfer destination on swap chain images if supported
		if (swapChainDetails.surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
			swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}*/

		// Create swapchain
		VkResult result = vkCreateSwapchainKHR(getLogicalDevice(), &swapChainCreateInfo, nullptr, &swapChain);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create swapchain");
		}
		else {
			std::cout << "Created the vulkan swapchain!\n";
		}

		// Recreation of the swapchain, cleanup the previous images.
		/*
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			for (uint32_t i = 0; i < swapChainImageCount; i++)
			{
				vkDestroyImageView(getLogicalDevice(), swapChainImages[i].imageView, nullptr);
			}
			vkDestroySwapchainKHR(getLogicalDevice(), oldSwapchain, nullptr);
		}*/

		// Get the swapchain images
		vkGetSwapchainImagesKHR(getLogicalDevice(), swapChain, &swapChainImageCount, nullptr);
		std::vector<VkImage> images(swapChainImageCount);
		vkGetSwapchainImagesKHR(getLogicalDevice(), swapChain, &swapChainImageCount, images.data());

		for (VkImage image : images) {

			// Store the image handle
			SwapChainImage swapChainImage = {};
			swapChainImage.image = image;
			swapChainImage.imageView = createImageView(logicalDevice->getLogicalDevice(), image, surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);

			// Add image to swapChainImages List in global namespace
			swapChainImages.push_back(swapChainImage);
		}

		// Allocate command buffers (same amount as swapchain images)
		commandBuffers.resize(swapChainImageCount);
		VkCommandBufferAllocateInfo cmdBufferAllocateInfo = {};
		cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufferAllocateInfo.commandPool = logicalDevice->getGraphicsCommandPool();
		cmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		result = vkAllocateCommandBuffers(getLogicalDevice(), &cmdBufferAllocateInfo, commandBuffers.data());
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate Command Buffers!");
		}
		std::cout << "Allocated " << swapChainImageCount << " command buffers from device command pool\n";

		// Create renderpass
		renderPass = new VulkanRenderpass(logicalDevice, surfaceFormat.format);

		// Framebuffer creation
		swapChainFramebuffers.resize(swapChainImages.size());
		size_t i = 0;
		for (; i < swapChainFramebuffers.size(); i++)
		{
			std::array<VkImageView, 2> attachments = {
				swapChainImages[i].imageView,
				depthStencil.imageView
			};

			VkFramebufferCreateInfo frameBufferCreateInfo = vks::initializers::framebufferCreateInfo();
			frameBufferCreateInfo.renderPass = renderPass->getRenderPass();
			frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			frameBufferCreateInfo.pAttachments = attachments.data();
			frameBufferCreateInfo.width = swapChainExtent.width;
			frameBufferCreateInfo.height = swapChainExtent.height;
			frameBufferCreateInfo.layers = 1;

			VkResult result = vkCreateFramebuffer(getLogicalDevice(), &frameBufferCreateInfo, nullptr, &swapChainFramebuffers[i]);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to create a framebuffer");
			}
		}
		std::cout << "Created " << i << " framebuffers\n";
	}

	SwapChainDetails VulkanSwapchain::getSwapchainDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface)
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

	VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
	{
		int width, height;
		glfwGetFramebufferSize(Application::get().getWindow().getWindow(), &width, &height); // Get width and height from window (glfw)

		if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) // If current extend is at numeric limits, the extend van vary. Otherwise it is the size of the window.
		{
			width = surfaceCapabilities.currentExtent.width;
			height = surfaceCapabilities.currentExtent.height;
			return surfaceCapabilities.currentExtent;
		}
		else {
			// If value can vary, need to set manually
			// Create new extent using window size
			VkExtent2D newExtent = {};
			newExtent.width = static_cast<uint32_t>(width);
			newExtent.height = static_cast<uint32_t>(height);

			// Surface also defines max and min, make sure inside boundaries by clamping value
			newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width)); // Keeps width within the boundaries
			newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));

			return newExtent;
		}
	}

	void VulkanSwapchain::waitForFrences()
	{
		vkWaitForFences(getLogicalDevice(), 1, &drawFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(getLogicalDevice(), 1, &drawFences[currentFrame]); // Unsignal fence (close it so other frames can't enter)
	}

	void VulkanSwapchain::createDepthStencil()
	{
		// Create the depth buffer image
		depthStencil.image = createImage(getPhysicalDevice(), getLogicalDevice(), swapChainExtent.width, swapChainExtent.height, 1, logicalDevice->getPhysicalDevice()->getDepthFormat(),
			VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthStencil.imageMemory, VK_SAMPLE_COUNT_1_BIT);

		//transitionImageLayout(mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, depthStencilImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
		//generateMipmaps(mainDevice, graphicsQueue, graphicsCommandPool, depthStencilImage, VK_FORMAT_D16_UNORM, swapChainExtent.width, swapChainExtent.height, mipLevels);

		// Create depth buffer image view
		depthStencil.imageView = createImageView(getLogicalDevice(), depthStencil.image, logicalDevice->getPhysicalDevice()->getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void VulkanSwapchain::createSynchronisation()
	{
		imageAvailable.resize(MAX_FRAME_DRAWS);
		renderFinished.resize(MAX_FRAME_DRAWS);
		drawFences.resize(MAX_FRAME_DRAWS);

		// Semaphore creation information
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		// Fence creation information
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAME_DRAWS; i++) {
			if (vkCreateSemaphore(getLogicalDevice(), &semaphoreCreateInfo, nullptr, &imageAvailable[i]) != VK_SUCCESS ||
				vkCreateSemaphore(getLogicalDevice(), &semaphoreCreateInfo, nullptr, &renderFinished[i]) != VK_SUCCESS ||
				vkCreateFence(getLogicalDevice(), &fenceCreateInfo, nullptr, &drawFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create a sempahore and/or Fence!");
			}
			std::cout << "Created index count " << i+1 << " synchronisation object!\n";
		}
	}

	void VulkanSwapchain::cleanup()
	{
		recreateSwapChain();
	}

	void VulkanSwapchain::recreateSwapChain()
	{
		vkDeviceWaitIdle(getLogicalDevice());

		vkFreeCommandBuffers(getLogicalDevice(), logicalDevice->getGraphicsCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		for (auto frameBuffer : swapChainFramebuffers)
		{
			vkDestroyFramebuffer(getLogicalDevice(), frameBuffer, nullptr);
		}

		renderPass->cleanup();
		delete renderPass;

		vkDestroyImage(getLogicalDevice(), depthStencil.image, nullptr);
		vkDestroyImageView(getLogicalDevice(), depthStencil.imageView, nullptr);
		vkFreeMemory(getLogicalDevice(), depthStencil.imageMemory, nullptr);

		for (auto image : swapChainImages) {
			//vkDestroyImage(getLogicalDevice(), image.image, nullptr);
			vkDestroyImageView(getLogicalDevice(), image.imageView, nullptr);
		}
		swapChainImages.clear();
		vkDestroySwapchainKHR(getLogicalDevice(), swapChain, nullptr);

		// Recreate
		create(vsync);
	}

	VulkanSwapchain::~VulkanSwapchain()
	{

	}
}
