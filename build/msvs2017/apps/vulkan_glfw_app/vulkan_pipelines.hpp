#pragma once

#include <vktoolkit.hpp>

// vertex structure
typedef struct VertexStruct_P4_C4_T2 {
	float X, Y, Z, W; 
	float R, G, B, A; 
	float U, V; 
} VertexStruct_P4_C4_T2;

// pipelines

void createPipeline_default(
	VulkanDevice&   device,
	VkRenderPass    renderPass,
	uint32_t        subPass,
	VulkanPipeline& pipeline);

void createPipeline_obj(
	VulkanDevice&   device,
	VkRenderPass    renderPass,
	uint32_t        subPass,
	VulkanPipeline& pipeline);
