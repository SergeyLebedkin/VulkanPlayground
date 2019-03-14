#include "vulkan_model.hpp"
#include "vulkan_context.hpp"

// VulkanModel::VulkanModel
VulkanModel::VulkanModel(VulkanContext& context) :
	VulkanContextObject(context), matrixModel(1.0f), visible(VK_TRUE), visibleDebug(VK_FALSE)
{
	// create model matrix buffer
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(glm::mat4), &bufferModelMatrix);
	// create descriptor set
	vulkanDescriptorSetCreate(context.device, context.descriptorSetLayout_model, &descriptorSet);
	// update descriptor set
	vulkanDescriptorSetUpdateBufferUniform(context.device, descriptorSet, bufferModelMatrix, 0);
}

// VulkanModel::~VulkanModel
VulkanModel::~VulkanModel()
{
	// destroy descriptor set
	vulkanDescriptorSetDestroy(context.device, descriptorSet);
	// destroy model matrix buffer
	vulkanBufferDestroy(context.device, bufferModelMatrix);
}

// VulkanModel::update
void VulkanModel::update(VulkanCommandBuffer& commandBuffer)
{
	// update model matrix
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferModelMatrix.buffer, 0, sizeof(matrixModel), &matrixModel);
	// update meshes
	for (auto& mesh : meshes) {
		if (mesh->material) 
			mesh->material->update(commandBuffer);
		mesh->update(commandBuffer);
	}
	// update debug meshes
	for (auto& mesh : meshes_debug) {
		if (mesh->material)
			mesh->material->update(commandBuffer);
		mesh->update(commandBuffer);
	}
}

// VulkanModel::draw
void VulkanModel::bind(VulkanCommandBuffer& commandBuffer)
{
	// bind descriptor set
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipelineLayout.pipelineLayout, 1, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
}