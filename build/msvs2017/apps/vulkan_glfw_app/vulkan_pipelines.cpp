#include "vulkan_pipelines.hpp"

// createPipeline_default
void createPipeline_default(
	VulkanDevice&   device, 
	VkRenderPass    renderPass, 
	uint32_t        subPass,
	VulkanPipeline& pipeline)
{
	// VkVertexInputBindingDescription
	VkVertexInputBindingDescription vertexBindingDescriptions[]
	{
	{ 0, sizeof(VertexStruct_P4_C4_T2), VK_VERTEX_INPUT_RATE_VERTEX },
	};

	// VkVertexInputAttributeDescription
	VkVertexInputAttributeDescription vertexInputAttributeDescriptions[]
	{
	{ 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT,  0 }, // position
	{ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16 }, // color
	{ 2, 0, VK_FORMAT_R32G32_SFLOAT      , 32 }, // texCoord
	};

	// VkDescriptorSetLayoutBinding
	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[]
	{
	{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_NULL_HANDLE }, // texture
	{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_VERTEX_BIT  , VK_NULL_HANDLE }, // buffer
	};

	// VkPipelineColorBlendAttachmentState
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentStates[]{
		{ // first attachments
			VK_FALSE,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		}
	};

	// create pipeline
	vulkanPipelineCreate(device, renderPass, subPass,
		"shaders/base.vert.spv", "shaders/base.frag.spv",
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions), vertexBindingDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions), vertexInputAttributeDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings), descriptorSetLayoutBindings,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates), pipelineColorBlendAttachmentStates,
		&pipeline);
}

// createPipeline_obj
void createPipeline_obj(
	VulkanDevice&   device,
	VkRenderPass    renderPass,
	uint32_t        subPass,
	VulkanPipeline& pipeline)
{
	// VkVertexInputBindingDescription
	VkVertexInputBindingDescription vertexBindingDescriptions[]
	{
	{ 0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX }, // position
	{ 1, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX }, // texCoord
	{ 2, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX }, // normal
	};

	// VkVertexInputAttributeDescription
	VkVertexInputAttributeDescription vertexInputAttributeDescriptions[]
	{
	{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 }, // position
	{ 1, 1, VK_FORMAT_R32G32_SFLOAT,    0 }, // texCoord
	{ 2, 2, VK_FORMAT_R32G32B32_SFLOAT, 0 }, // normal
	};

	// VkDescriptorSetLayoutBinding
	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[]
	{
	{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_NULL_HANDLE }, // texture
	{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_VERTEX_BIT  , VK_NULL_HANDLE }, // buffer
	};

	// VkPipelineColorBlendAttachmentState
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentStates[]{
		{ // first attachments
			VK_FALSE,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		}
	};

	// create pipeline
	vulkanPipelineCreate(device, renderPass, subPass,
		"shaders/obj.vert.spv", "shaders/obj.frag.spv",
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions), vertexBindingDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions), vertexInputAttributeDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings), descriptorSetLayoutBindings,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates), pipelineColorBlendAttachmentStates,
		&pipeline);
}
