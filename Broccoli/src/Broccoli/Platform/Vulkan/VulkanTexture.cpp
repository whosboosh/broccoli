#include "VulkanTexture.h"

#include "Broccoli/Platform/Vulkan/VulkanContext.h"

namespace Broccoli {
	VulkanTexture::VulkanTexture(const std::string& fileName)
	{
		VulkanLogicalDevice* logicalDevice = VulkanContext::get()->getLogicalDevice();

		stbi_uc* imageData = loadTextureFile(fileName, &width, &height, &imageSize);
		mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		VkBuffer imageStagingBuffer;
		VkDeviceMemory imageStagingBufferMemory;
		createBuffer(logicalDevice->getPhysicalDevice()->getVulkanPhysicalDevice(), logicalDevice->getLogicalDevice(), imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&imageStagingBuffer, &imageStagingBufferMemory);

		// Copy image data to staging buffer
		void* data;
		vkMapMemory(logicalDevice->getLogicalDevice(), imageStagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, imageData, static_cast<size_t>(imageSize));
		vkUnmapMemory(logicalDevice->getLogicalDevice(), imageStagingBufferMemory);

		stbi_image_free(imageData);

		// Create image to hold final texture
		VkImage texImage;
		VkDeviceMemory texImageMemory;
		texImage = createImage(logicalDevice->getPhysicalDevice()->getVulkanPhysicalDevice(), logicalDevice->getLogicalDevice(), 
			width, height, mipLevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texImageMemory, VK_SAMPLE_COUNT_1_BIT);

		// Transition image to be DST for copy operation
		transitionImageLayout(logicalDevice->getLogicalDevice(), logicalDevice->getGraphicsQueue(), logicalDevice->getGraphicsCommandPool(), texImage,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

		// Copy data to image (Staging buffer to texImage)
		copyImageBuffer(logicalDevice->getLogicalDevice(), logicalDevice->getGraphicsQueue(), logicalDevice->getGraphicsCommandPool(), imageStagingBuffer, texImage, width, height);

		// Transition image to be shader readable for shader usage
		//transitionImageLayout(mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, texImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

		generateMipmaps(logicalDevice->getLogicalDevice(), logicalDevice->getPhysicalDevice()->getVulkanPhysicalDevice(), logicalDevice->getGraphicsQueue(), logicalDevice->getGraphicsCommandPool(), 
			texImage, VK_FORMAT_R8G8B8A8_SRGB, width, height, mipLevels);



	}
	uint32_t VulkanTexture::getHeight()
	{
		return height;
	}
	uint32_t VulkanTexture::getWidth()
	{
		return width;
	}
}