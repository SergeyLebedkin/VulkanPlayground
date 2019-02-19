#include "vulkan_pipelines.hpp"

// createPipeline_default
void createPipeline_default(
	VulkanDevice&   device, 
	VkRenderPass    renderPass, 
	uint32_t        subPass,
	VulkanPipeline& pipeline)
{
	// create pipeline
	vulkanPipelineCreate(device, renderPass, subPass,
		"shaders/base.vert.spv", "shaders/base.frag.spv",
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_C4_T2), vertexBindingDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_C4_T2), vertexInputAttributeDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_T1_U1), descriptorSetLayoutBindings_T1_U1,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline);
}

// createPipeline_obj
void createPipeline_obj(
	VulkanDevice&   device,
	VkRenderPass    renderPass,
	uint32_t        subPass,
	VulkanPipeline& pipeline)
{
	// create pipeline
	vulkanPipelineCreate(device, renderPass, subPass,
		"shaders/obj.vert.spv", "shaders/obj.frag.spv",
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_T1_U1), descriptorSetLayoutBindings_T1_U1,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline);
}
