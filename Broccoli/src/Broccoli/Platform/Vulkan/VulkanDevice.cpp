#include "VulkanDevice.h"

// Same directory platform includes
#include "VulkanContext.h"

#include <set>

namespace Broccoli {
	VulkanPhysicalDevice::VulkanPhysicalDevice()
	{
		std::cout << "Vulkan Physical Device Creation\n";
		// Enumerate the physical devices the vkInstance can access
		// Get the count of devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(VulkanContext::getInstance(), &deviceCount, nullptr);
		// If deviceCount = 0 then no devices support vulkan
		if (deviceCount == 0)
		{
			throw std::runtime_error("Can't find GPUs that suport Vulkan Instance\n");
		}

		// Get list of physical devices
		std::vector<VkPhysicalDevice> deviceList(deviceCount);
		vkEnumeratePhysicalDevices(VulkanContext::getInstance(), &deviceCount, deviceList.data());

		// TEMP: get first device
		VkPhysicalDevice selectedPhysicalDevice = nullptr;
		for (VkPhysicalDevice device : deviceList)
		{
			if (checkDeviceSuitable(device))
			{
				selectedPhysicalDevice = device;
				break;
				//msaaSamples = getMaxUseableSampleCount();
				//msaaSamples = translateSampleIntToEnum(8);
			}
		}
		if (!selectedPhysicalDevice)
		{
			throw std::runtime_error("No discrete GPU found\n");
		}
		physicalDevice = selectedPhysicalDevice;
		// TODO: depth format
		depthFormat = chooseSupportedFormat(
			{ VK_FORMAT_D16_UNORM, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
	}

	bool VulkanPhysicalDevice::checkDeviceSuitable(VkPhysicalDevice device)
	{
		
		// Information about the device itself(ID, name, type, vendor, etc)
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		std::cout << "Vulkan Physical Device properties fetched...\n";


		// No discrete GPU
		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return false;

		// Save the min uniform offset for later use (dynamic buffers)
		minUniformBufferOffset = deviceProperties.limits.minUniformBufferOffsetAlignment;

		// Information about what the device can do (geo shader, tess shader, wide lines etc)
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		std::cout << "Vulkan physical device features fetched...\n";


		// Check what Queues are supported
		// indicies is private to VulkanPhysicalDevice
		deviceIndices = getQueueFamilies(device);
		std::cout << "Vulkan physical device queue families fetched...\n";

		// Check that the device extentions are valid
		// deviceExtensions defined in VulkanDevice.h
		bool extensionsSupported = checkDeviceExtensionSupport(device, deviceExtensions);
		std::cout << "Vulkan physical device extention support: " << extensionsSupported << "\n";
		std::cout << "Vulkan device indicies valid: " << deviceIndices.isValid() << "\n";
		std::cout << "Vulkan device sampler anisotropy support: " << deviceFeatures.samplerAnisotropy << "\n";

		// TODO:
		/*
		if (extensionsSupported) {
			SwapChainDetails swapChainDetails = getSwapChainDetails(device);
			swapChainValid = !swapChainDetails.presentationMode.empty() && !swapChainDetails.formats.empty();
		}*/

		return deviceIndices.isValid() && extensionsSupported && deviceFeatures.samplerAnisotropy;
	}

	QueueFamilyIndicies VulkanPhysicalDevice::getQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndicies indicies;

		// Get all Queue Family Property info for the given device
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

		// Go through each queue family, check if it has atleast 1 of the required types of queue
		int i = 0;
		for (const auto& queueFamily : queueFamilyList)
		{
			// First check if queue family has at least 1 in family, queue can be multiple types. Find through bitwise & with VK_QUEUE_*_BIT to check if it has requried type
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indicies.graphicsFamily = i; // If queue family is valid, get the index
			}

			// Check if the queue family supports presentation
			VkBool32 presentationSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, VulkanContext::getSurface(), &presentationSupport);
			// Check if queue is presentation type (can be both graphics and presentation)
			if (queueFamily.queueCount > 0 && presentationSupport == true)
			{
				indicies.presentationFamily = i;
			}


			// Check if the queue family indicies are in a valid state. Break
			if (indicies.isValid()) break;
			i++;
		}

		return indicies;
	}

	bool VulkanPhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtensions)
	{
		// Get length of supported extensions
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		// Populate vector with list of supported extensions
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

		// If no extensions are supported
		if (extensionCount == 0) return false;

		// Check the supported extensions against the extensions we wish to support
		for (const auto& deviceExtension : deviceExtensions) {
			bool hasExtension = false;
			for (const auto& extension : extensions) {
				if (strcmp(extension.extensionName, deviceExtension) == 0) {
					hasExtension = true;
					break;
				}
			}
			if (!hasExtension) {
				return false;
			}
		}
		return true;
	}

	VkFormat VulkanPhysicalDevice::chooseSupportedFormat(const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags)
	{
		// Loop through all formats and find a compatible one
		for (VkFormat format : formats) {
			// Get properties for given format on this device
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

			if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & featureFlags) == featureFlags) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & featureFlags) == featureFlags) {
				return format;
			}
		}

		throw std::runtime_error("Failed to find a matching format!");
	}

	VkSampleCountFlagBits translateSampleIntToEnum(int level)
	{
		VkSampleCountFlagBits temp;
		if (level % 2 == 0 || level == 1) {
			std::cout << level;
			if (level == 1) temp = VK_SAMPLE_COUNT_1_BIT;
			if (level == 2) temp = VK_SAMPLE_COUNT_2_BIT;
			if (level == 4) temp = VK_SAMPLE_COUNT_4_BIT;
			if (level == 8) temp = VK_SAMPLE_COUNT_8_BIT;
		}
		return temp;
	}

	VulkanLogicalDevice::VulkanLogicalDevice(const Ref<VulkanPhysicalDevice>& physicalDevice) : physicalDevice(physicalDevice)
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<int> queueFamilyIndicies = { physicalDevice->getQueueFamilyIndicies().graphicsFamily, physicalDevice->getQueueFamilyIndicies().presentationFamily }; // If they both are the same value, only 1 is stored
	
		// Queues the logical device needs to create and info to do so
		for (int queueFamilyIndex : queueFamilyIndicies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex; // Index of the family to create a queue from
			queueCreateInfo.queueCount = 1;						// Number of queues to create
			float priority = 1.0f;
			queueCreateInfo.pQueuePriorities = &priority;		// Vulkan needs to know how to handle multiple queue families, (1 is the highest priority)

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Information to create logical device, (sometimes called "device")
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());  // Number of Queue Create Infos
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();							 // List of Queue Create Infos so device can create required queues
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); // Number of enabled logical device extensions
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();						 // List of enabled logical device extensions

		// Physical device features the logical device will be using
		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE; // Enable anisotropy
		//deviceFeatures.depthClamp = VK_TRUE; // use if using depthClampEnable to true
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures; // Physical Device features Logical Device will use

		// Create the logical device for the given phyiscal device
		VkResult result = vkCreateDevice(physicalDevice->getVulkanPhysicalDevice(), &deviceCreateInfo, nullptr, &logicalDevice);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create a Logical Device\n");
		}

		// Queues are created at the same time as the device, we need a handle to queues
		vkGetDeviceQueue(logicalDevice, physicalDevice->getQueueFamilyIndicies().graphicsFamily, 0, &graphicsQueue); // From Logical Device, of given Queue Family, of Queue Index(0). Store queue reference in the graphicsQueue
		vkGetDeviceQueue(logicalDevice, physicalDevice->getQueueFamilyIndicies().presentationFamily, 0, &presentationQueue);
		std::cout << "Graphics and presentation queue fetched...\n";

		// Create command pools
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = physicalDevice->getQueueFamilyIndicies().graphicsFamily; // Queue family type that buffers from this command pool will use

		// Create a graphics queue family command pool
		result = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &graphicsCommandPool);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create graphics pool (Command Pool)\n");
		}
		else {
			std::cout << "Graphics command pool created successfully\n";
		}

		std::cout << "Vulkan Logical device creation successful\n";
	}

	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		cleanup();
	}

	void VulkanLogicalDevice::cleanup()
	{
		vkDestroyCommandPool(logicalDevice, graphicsCommandPool, nullptr);

		vkDeviceWaitIdle(logicalDevice);
		vkDestroyDevice(logicalDevice, nullptr);
	}

}