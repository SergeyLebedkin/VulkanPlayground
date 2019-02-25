#pragma once

#include "vulkan_meshes.hpp"
#include <vector>

// VulkanModel
class VulkanModel {
protected:
	VulkanDevice&         device;
	VulkanPipelineLayout& pipelineLayout;
protected:
	VulkanBuffer        bufferModelMatrix;
	VulkanDescriptorSet descriptorSet;
public:
	std::vector<VulkanMesh*> meshes{};
	std::vector<VulkanMesh*> meshesDebug{};
	glm::mat4                matModel;
	VkBool32                 visible{};
	VkBool32                 visibleDebug{};
public:
	VulkanModel(
		VulkanDevice&              device,
		VulkanPipelineLayout&      pipelineLayout,
		VulkanDescriptorSetLayout& descriptorSetLayout);
	virtual ~VulkanModel();
	virtual void draw(VulkanCommandBuffer& commandBuffer);
};
