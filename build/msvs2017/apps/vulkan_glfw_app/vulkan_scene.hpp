#pragma once

#include "vulkan_model.hpp"
#include <vector>

// VulkanScene
class VulkanScene {
protected:
	VulkanDevice&         device;
	VulkanPipelineLayout& pipelineLayout;
protected:
	VulkanBuffer        bufferViewProjMatrices;
	VulkanDescriptorSet descriptorSet;
public:
	std::vector<VulkanModel*> models{};
	glm::mat4                 matView;
	glm::mat4                 matProj;
public:
	VulkanScene(
		VulkanDevice&              device,
		VulkanPipelineLayout&      pipelineLayout,
		VulkanDescriptorSetLayout& descriptorSetLayout);
	virtual ~VulkanScene();
	virtual void draw(VulkanCommandBuffer& commandBuffer);
};
