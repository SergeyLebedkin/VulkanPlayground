#pragma once
#include "vulkan_pipelines.hpp"

// VulkanMesh
struct VulkanMesh
{
	virtual ~VulkanMesh() {};
	virtual void draw(
		VulkanDevice&        device,
		VulkanCommandBuffer& commandBuffer) = 0;
};

// VulkanMesh
struct VulkanMesh_obj : public VulkanMesh
{
	int32_t      vertexCount;
	VulkanBuffer bufferPos;
	VulkanBuffer bufferTex;
	VulkanBuffer bufferNrm;
	VulkanImage  image;

	// external
	VulkanDevice&   device;
	VulkanPipeline& pipeline;
	VulkanSampler&  sampler;

	// constructor
	VulkanMesh_obj(
		VulkanDevice&   device,
		VulkanPipeline& pipeline,
		VulkanSampler&  sampler);
	~VulkanMesh_obj();
	void draw(
		VulkanDevice&        device,
		VulkanCommandBuffer& commandBuffer) override;
};
