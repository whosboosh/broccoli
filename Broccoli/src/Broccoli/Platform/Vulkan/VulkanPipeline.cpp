#include "VulkanPipeline.h"

#include "Broccoli/Utilities/VulkanUtilities.h"
#include "Broccoli/Utilities/VulkanInitializers.hpp"

#include "Broccoli/Core/Application.h"

#include "VulkanSwapChain.h"
#include "VulkanRenderpass.h"

#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Platform/Vulkan/VulkanShader.h"
#include "Broccoli/Platform/Vulkan/VulkanContext.h"

#include "Broccoli/Renderer/VertexBuffer.h"

namespace Broccoli {

	VulkanPipeline::VulkanPipeline(const PipelineSpecification& spec, ShaderLibrary* shaderLibrary)
	{
		this->shaderLibrary = shaderLibrary;
		this->spec = spec;

		create();
	}

	void VulkanPipeline::create()
	{
		VkDevice logicalDevice = VulkanContext::get()->getLogicalDevice()->getLogicalDevice();
		VulkanSwapchain& swapChain = VulkanContext::get()->getVulkanSwapChain();
		VulkanRenderpass renderPass = swapChain.getRenderPass();
		std::vector<VkFramebuffer> swapChainFramebuffers = swapChain.getSwapChainFrameBuffers();

		//std::cout << "\n\n\n"<< shaderLibrary->getCurrentShaders().at(0).As<Shader>()->getName()<<"\n";

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
		vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()); // TODO: REMOVE THE 0
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
		if (spec.topology == PrimitiveTopology::Lines || spec.topology == PrimitiveTopology::LineStrip || spec.wireFrame)
			dyanamicStateEnables.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);

		// Dynamic state creation info
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dyanamicStateEnables.size());
		dynamicStateCreateInfo.pDynamicStates = dyanamicStateEnables.data();


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
		colorBlendAttachment.blendEnable = VK_TRUE;

		// Blending uses equation: (srcColorBlendFactor * new colour) colorBlendOp (dstColorBlendFactor * old colour)
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

		// Summarised: (VK_BLEND_FACTOR_SRC_ALPHA * new colour) + (VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA * old colour)
		//			   (new colour alpha * new colour) + ((1 - new colour alpha) * old colour)

		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		// Summarised: (1 * new alpha) + (0 * old alpha) = new alpha

		VkPipelineColorBlendStateCreateInfo colourBlendingCreateInfo = {};
		colourBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colourBlendingCreateInfo.logicOpEnable = VK_FALSE;				// Alternative to calculations is to use logical operations
		colourBlendingCreateInfo.attachmentCount = 1;
		colourBlendingCreateInfo.pAttachments = &colorBlendAttachment;

		// Descriptor sets and push constants
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {};
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {};

		for (std::pair<std::string, Ref<Shader>> shader : shaderLibrary->getCurrentShaders())
		{
			std::vector<ShaderDescriptorSet> shaderDescriptors = shader.second.As<VulkanShader>()->getShaderDescriptorSets();

			for (int i = 0; i < shaderDescriptors.size(); i++)
			{
				descriptorSetLayouts.push_back(shaderDescriptors[i].descriptorSetLayout);
			}

			shaderStages.push_back(shader.second.As<VulkanShader>()->getShaderStageInfo());
		}

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
		multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // Number of samples to use per fragment
		multisampleCreateInfo.sampleShadingEnable = VK_FALSE; // enable sample shading in the pipeline
		multisampleCreateInfo.minSampleShading = 1.0f; // min fraction for sample shading; closer to one is smooth

		// Create Graphics Pipeline
		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = 2; // Number of shader stages (vertex, fragment)
		pipelineCreateInfo.pStages = shaderStages.data(); // List of shader stages // TODO: Add.data() to shaderStages for original
		pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo; // All the fixed function pipeline states
		pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizerStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
		pipelineCreateInfo.pColorBlendState = &colourBlendingCreateInfo;
		pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
		pipelineCreateInfo.layout = pipelineLayout; // Pipeline layout pipeline should use
		pipelineCreateInfo.renderPass = renderPass.getRenderPass(); // Render pass description the pipeline is compatible with
		pipelineCreateInfo.subpass = 0; // Subpass of render pass to use with pipeline

		// Pipeline derivatives: Can create multiple pipelines that derive from one another for optimisation
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Existing pipeline to derive from
		pipelineCreateInfo.basePipelineIndex = -1; // Or index of pipeline being created to derive from (in case creating multiple at once) 

		result = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create scene graphics pipeline");
		}
		std::cout << "Pipeline created!\n";
	}

	VulkanPipeline::~VulkanPipeline()
	{
		//cleanup();
	}
	void VulkanPipeline::cleanup()
	{
		recreateSwapChain();
		shaderLibrary->cleanup();
	}
	void VulkanPipeline::recreateSwapChain()
	{
		VkDevice logicalDevice = VulkanContext::get()->getLogicalDevice()->getLogicalDevice();
		vkDeviceWaitIdle(logicalDevice);

		vkDestroyPipeline(logicalDevice, pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);

		// Recreate the pipeline
		create();
	}
}