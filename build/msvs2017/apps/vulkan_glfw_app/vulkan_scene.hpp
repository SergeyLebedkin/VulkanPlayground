#pragma once

#include "vulkan_model.hpp"
#include <vector>

// VulkanScene
class VulkanScene {
protected:
	VulkanContext& context;
protected:
	VulkanBuffer        bufferViewProjMatrices;
	VulkanDescriptorSet descriptorSet{};
public:
	std::vector<VulkanModel*> models{};
	glm::mat4                 matView;
	glm::mat4                 matProj;
public:
	// constructor and destructor
	VulkanScene(VulkanContext& context);
	virtual ~VulkanScene();

	// bind
	virtual void bind(VulkanCommandBuffer& commandBuffer);

	// render
	virtual void beforeRender(VulkanCommandBuffer& commandBuffer);
	virtual void afterRender(VulkanCommandBuffer& commandBuffer);
};
