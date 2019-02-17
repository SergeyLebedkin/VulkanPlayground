#pragma once
#include "vulkan_pipelines.hpp"

// VulkanMesh
struct VulkanMesh
{
	virtual ~VulkanMesh() {};
	virtual void draw(VulkanPipeline& pipeline, VulkanCommandBuffer& commandBuffer) = 0;
};

// VulkanMesh
struct VulkanMesh_obj : public VulkanMesh
{
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferTex{};
	VulkanBuffer bufferNrm{};
	int32_t      vertexCount;

	// material
	VulkanSampler* sampler{};
	VulkanImage*   image{};

	// general
	VulkanDevice& device;

	// constructor
	VulkanMesh_obj(VulkanDevice& device);
	~VulkanMesh_obj();
	void draw(VulkanPipeline& pipeline, VulkanCommandBuffer& commandBuffer) override;
};
