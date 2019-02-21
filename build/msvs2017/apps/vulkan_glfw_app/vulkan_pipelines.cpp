#include "vulkan_pipelines.hpp"

// createPipeline_default
void createPipeline_default(
	VulkanDevice&   device, 
	VkRenderPass    renderPass, 
	uint32_t        subPass,
	VulkanShader&   shader,
	VulkanPipeline& pipeline)
{
	// create shader
	vulkanShaderCreate(device, "shaders/base.vert.spv", "shaders/base.frag.spv",
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_T1_U1), descriptorSetLayoutBindings_T1_U1,
		&shader);

	// create pipeline
	vulkanPipelineCreate(device, shader, renderPass, subPass,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_C4_T2), vertexBindingDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_C4_T2), vertexInputAttributeDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline);
}

// createPipeline_obj
void createPipeline_obj(
	VulkanDevice&   device,
	VkRenderPass    renderPass,
	uint32_t        subPass,
	VulkanShader&   shader,
	VulkanPipeline& pipeline,
	VulkanPipeline& pipeline_wf)
{
	// create shader
	vulkanShaderCreate(device, "shaders/obj.vert.spv", "shaders/obj.frag.spv",
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_T1_U1), descriptorSetLayoutBindings_T1_U1,
		&shader);

	// create pipeline
	vulkanPipelineCreate(device, shader, renderPass, subPass,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline);

	// create pipeline wire frame
	vulkanPipelineCreate(device, shader, renderPass, subPass,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_LINE,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_wf);
}

// createPipeline_line
void createPipeline_line(
	VulkanDevice&   device,
	VkRenderPass    renderPass,
	uint32_t        subPass,
	VulkanShader&   shader,
	VulkanPipeline& pipeline)
{
	// create shader
	vulkanShaderCreate(device, "shaders/line.vert.spv", "shaders/line.frag.spv",
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_U1), descriptorSetLayoutBindings_U1,
		&shader);

	// create pipeline
	vulkanPipelineCreate(device, shader, renderPass, subPass,
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_C4), vertexBindingDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_C4), vertexInputAttributeDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline);
}
