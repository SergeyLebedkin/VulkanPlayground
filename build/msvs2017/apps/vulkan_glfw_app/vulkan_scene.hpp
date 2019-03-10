#pragma once

#include "vulkan_model.hpp"

// VulkanScene
class VulkanScene : public VulkanContextObject {
protected:
	// scene descriptor set
	VulkanDescriptorSet descriptorSet{};
protected:
	// view-projection matrices buffer
	VulkanBuffer bufferViewProjectionMatrices;
public:
	// models
	std::vector<VulkanModel*> models{};
public:
	// view and projection matrices
	glm::mat4 matrixView;
	glm::mat4 matrixProjection;
public:
	// constructor and destructor
	VulkanScene(VulkanContext& context);
	~VulkanScene();

	// update
	void update(VulkanCommandBuffer& commandBuffer) override;

	// bind
	virtual void bind(VulkanCommandBuffer& commandBuffer);
};
