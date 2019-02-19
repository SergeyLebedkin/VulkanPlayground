#pragma once
#include <glm/mat4x4.hpp>
#include "vulkan_pipelines.hpp"

// VulkanMesh
struct VulkanMesh
{
	virtual ~VulkanMesh() {};
	virtual void draw(
		VulkanPipeline&      pipeline,
		VulkanCommandBuffer& commandBuffer,
		glm::mat4&           matProj,
		glm::mat4&           matView,
		glm::mat4&           matModl) = 0;
};

// VulkanMesh
struct VulkanMesh_obj : public VulkanMesh
{
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferTex{};
	VulkanBuffer bufferNrm{};
	VulkanBuffer bufferMVP{};
	int32_t      vertexCount;

	bool matwrite = false;

	// material
	VulkanDescriptorSet descriptorSet{};

	// general
	VulkanDevice& device;

	// constructor
	VulkanMesh_obj(VulkanDevice& device);
	~VulkanMesh_obj();
	void draw(
		VulkanPipeline&      pipeline,
		VulkanCommandBuffer& commandBuffer,
		glm::mat4&           matProj,
		glm::mat4&           matView,
		glm::mat4&           matModl) override;
};
