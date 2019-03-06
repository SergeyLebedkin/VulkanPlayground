#pragma once

#include "vulkan_meshes.hpp"
#include <glm/mat4x4.hpp>
#include <vector>

// VulkanModel
class VulkanModel {
protected:
	// context
	VulkanContext& context;
protected:
	VulkanBuffer        bufferModelMatrix;
	VulkanDescriptorSet descriptorSet;
public:
	// meshes
	std::vector<VulkanMesh*>       meshes{};
	std::vector<VulkanMesh_debug*> meshesDebug{};
public:
	VkBool32 visible{};
	VkBool32 visibleDebug{};
public:
	glm::mat4 matModel;
public:
	// constructor and destructor
	VulkanModel(VulkanContext& context);
	virtual ~VulkanModel();

	// bind
	virtual void bind(VulkanCommandBuffer& commandBuffer);

	// render
	virtual void beforeRender(VulkanCommandBuffer& commandBuffer);
	virtual void afterRender(VulkanCommandBuffer& commandBuffer);
};
