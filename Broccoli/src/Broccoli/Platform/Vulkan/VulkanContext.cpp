#pragma once

#include "VulkanContext.h"

#include <iostream>

namespace Broccoli {
	VulkanContext::VulkanContext()
	{
	}
	VulkanContext::~VulkanContext()
	{
	}
	void VulkanContext::init()
	{
		std::cout << "Vulkan running!";
	}
}
