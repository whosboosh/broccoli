#include "VulkanPipeline.h"

#include "Broccoli/Utilities/VulkanUtilities.h"
#include "Broccoli/Utilities/VulkanInitializers.hpp"

#include "Broccoli/Core/Application.h"

#include "VulkanSwapChain.h"
#include "VulkanRenderpass.h"

#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Platform/Vulkan/VulkanShader.h"
#include "Broccoli/Platform/Vulkan/VulkanContext.h"

namespace Broccoli {

	VulkanPipeline::VulkanPipeline(const PipelineSpecification& spec, ShaderLibrary* shaderLibrary)
	{
		VkDevice logicalDevice = Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice()->getLogicalDevice();
		VulkanSwapchain swapChain = Application::get().getWindow().getVulkanSwapChain();
		VulkanRenderpass renderPass = swapChain.getRenderPass();
		std::vector<VkFramebuffer> swapChainFramebuffers = swapChain.getSwapChainFrameBuffers();

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {};

		//std::cout << "\n\n\n"<< shaderLibrary->getCurrentShaders().at(0).As<Shader>()->getName()<<"\n";

		
		for (std::pair<std::string, Ref<Shader>> shader : shaderLibrary->getCurrentShaders())
		{
			//std::cout << shader.second.As<VulkanShader>()->getName();
			std::cout << shader.second.As<Shader>()->getName()<<"\n";
			shaderStages.push_back(shader.second.As<VulkanShader>()->getShaderStageInfo());
		}

		// Create the pipeline
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0; // Can bind multiple streams of data, this defines which one (first binding as only one string)
		bindingDescription.stride = sizeof(Vertex); // Size of a single vertex object, defines how big each vertex is and how to split them
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // How to move between data after each vertex, 


		// How the data for an attribute is defined within a vertex
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions;

		// Position attribute
		attributeDescriptions[0].binding = 0; // Which binding the data is at (should be the same as above binding value)
		attributeDescriptions[0].location = 0; // Location in shader where data will be read from
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // Format the data will take (also helps define the size of data)
		attributeDescriptions[0].offset = offsetof(Vertex, pos); // Where the attribute is defined in the data for a single vertex

		// Color Attribute
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, col);

		// Texture Attribute
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, tex);

		// Normal Attribute
		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, normal);

		// Vertex Input
		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = vks::initializers::pipelineVertexInputStateCreateInfo();
		vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
		vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription; // List of vertex binding descriptions (data spacing/stride information)
		vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // List of vertex attribute descriptions (data format and where to bind to/from)

		// Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = GetVulkanTopology(spec.topology); // Primitive type to assemble verticies
		inputAssembly.primitiveRestartEnable = VK_FALSE; // Allow overriding of "strip" topology to start new primitives

		// Viewport and Scissor
		VkViewport viewport = {};
		viewport.x = 0.0f; // x start coordinate
		viewport.y = 0.0f; // y start coordinate
		viewport.width = (float)swapChain.getSwapExtent().width; // width of viewport
		viewport.height = (float)swapChain.getSwapExtent().height; // height of viewport
		viewport.minDepth = 0.0f; // Min framebuffer depth
		viewport.maxDepth = 1.0f; // Max framebuffer depth

		// Create a scissor info struct
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 }; // Offset to use region from
		scissor.extent = swapChain.getSwapExtent(); // Extent to describe region to use, starting at offset

		// Viewport and scissor info struct
		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;

		// Dyanamic states to enable (Change values in runtime instead of hardcoding pipeline)
		std::vector<VkDynamicState> dyanamicStateEnables;
		dyanamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT); // Dynamic Viewport: Can resize in command buffer with vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		dyanamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR); // Dynamic Scissor: Can resize in command buffer with vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// Dynamic state creation info
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dyanamicStateEnables.size());
		dynamicStateCreateInfo.pDynamicStates = dyanamicStateEnables.data();
		dynamicStateCreateInfo.pNext = 0;


		// Rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizerStateCreateInfo = {};
		rasterizerStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerStateCreateInfo.depthClampEnable = VK_FALSE; // Change if fragments beyond near/far plane are clipped (default) or clamped to plane
		rasterizerStateCreateInfo.rasterizerDiscardEnable = VK_FALSE; // Whether to discard data and skip rasterizer. Never creates fragements
		rasterizerStateCreateInfo.polygonMode = spec.wireFrame ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL; // How to handle filling points between vertices
		rasterizerStateCreateInfo.lineWidth = spec.lineWidth; // The thickness of the lines when drawn (enable wide lines for anything other than 1.0f)
		rasterizerStateCreateInfo.cullMode = spec.backFaceCulling ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
		rasterizerStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Winding to determine which side is front
		rasterizerStateCreateInfo.depthBiasEnable = VK_FALSE; // Whether to add depth bias to fragments (good for stopping "shadow achne" in shadow mapping)

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		// Descriptor sets and push constants
		// TODO : Create descriptor sets in shader
		std::array<VkDescriptorSetLayout, 0> descriptorSetLayouts = { };

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;


		// Create Pipeline Layout
		VkResult result = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}

		// Depth Stencil Testing
		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
		depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilCreateInfo.depthTestEnable = spec.depthTest ? VK_TRUE : VK_FALSE; // Enable checking depth to determine fragment write
		depthStencilCreateInfo.depthWriteEnable = spec.depthWrite ? VK_TRUE : VK_FALSE;; // Enable writing to the depth buffer (to replace old values)
		depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS; // Comparision operation that allows an overwrite (is in front)
		depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE; // Depth bounds test: Does the depth value exist between 2 bounds (front, back)
		depthStencilCreateInfo.stencilTestEnable = VK_FALSE;

		// Multi sampling state
		VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {};
		multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleCreateInfo.pSampleMask = nullptr;

		// Create Graphics Pipeline
		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = 2; // Number of shader stages (vertex, fragment)
		pipelineCreateInfo.pStages = shaderStages.data(); // List of shader stages
		pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo; // All the fixed function pipeline states
		pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizerStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
		pipelineCreateInfo.pColorBlendState = &colorBlending;
		pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
		pipelineCreateInfo.layout = pipelineLayout; // Pipeline layout pipeline should use
		pipelineCreateInfo.renderPass = renderPass.getRenderPass(); // Render pass description the pipeline is compatible with
		pipelineCreateInfo.subpass = 0; // Subpass of render pass to use with pipeline

		result = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create scene graphics pipeline");
		}
		std::cout << "Pipeline created!";
	}
	VulkanPipeline::~VulkanPipeline()
	{
	}
}