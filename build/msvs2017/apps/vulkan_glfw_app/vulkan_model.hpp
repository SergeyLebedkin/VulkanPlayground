#pragma once

#include "vulkan_meshes.hpp"
#include <vector>

// VulkanModel
class VulkanModel {
public:
	std::vector<VulkanMesh*> meshes{};
	std::vector<VulkanMesh*> meshesDebug{};
	glm::mat4                matModel;
	VkBool32                 visible{};
	VkBool32                 visibleDebug{};

	VulkanModel() : visible(VK_TRUE), visibleDebug(VK_FALSE) {}
	~VulkanModel() {}
	void draw(
		VulkanPipeline&      pipeline,
		VulkanCommandBuffer& commandBuffer,
		glm::mat4&           matProj,
		glm::mat4&           matView);
};
