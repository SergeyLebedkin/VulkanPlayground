#pragma once

#include "vulkan_meshes.hpp"
#include <glm/mat4x4.hpp>
#include <vector>

// VulkanModel
class VulkanModel : public VulkanContextObject {
protected:
	// model descriptor set
	VulkanDescriptorSet descriptorSet{};
protected:
	// model matrix buffer
	VulkanBuffer bufferModelMatrix{};
public:
	// model matrix
	glm::mat4 matrixModel = glm::mat4(1.0f);
public:
	// meshes
	std::vector<VulkanMesh_material*> meshes{};
	std::vector<VulkanMesh_color*>    meshes_debug{};
public:
	VkBool32 visible{};
	VkBool32 visibleDebug{};
public:
	// constructor and destructor
	VulkanModel(VulkanContext& context);
	virtual ~VulkanModel();

	// update
	void update(VulkanCommandBuffer& commandBuffer) override;

	// bind
	virtual void bind(VulkanCommandBuffer& commandBuffer);
};
