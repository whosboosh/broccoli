#include "VulkanDevice.h"

// Same directory platform includes
#include "VulkanContext.h"

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
			throw std::runtime_error("Can't find GPUs that suport Vulkan Instance");
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
			throw std::runtime_error("No discrete GPU found");
		}
		physicalDevice = selectedPhysicalDevice;
		// TODO: depth format
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
	}

	Ref<VulkanPhysicalDevice> VulkanPhysicalDevice::selectDevice()
	{
		return Ref<VulkanPhysicalDevice>::create();
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
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		bool extensionsSupported = checkDeviceExtensionSupport(device, deviceExtensions);
		std::cout << "Vulkan physical device extention support: " << extensionsSupported << "\n";
		std::cout << "Vulkan device indicies valid: " << deviceIndices.isValid() << "\n";
		std::cout << "Vulkan device sampler anisotropy support: " << deviceFeatures.samplerAnisotropy << "\n";

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


	VulkanLogicalDevice::VulkanLogicalDevice()
	{
	}

	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
	}

}