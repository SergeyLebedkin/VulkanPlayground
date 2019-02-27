#include "vulkan_model.hpp"

// VulkanModel::VulkanModel
VulkanModel::VulkanModel(
	VulkanDevice&              device,
	VulkanPipelineLayout&      pipelineLayout,
	VulkanDescriptorSetLayout& descriptorSetLayout) :
	device(device), pipelineLayout(pipelineLayout),
	visible(VK_TRUE), visibleDebug(VK_TRUE)
{
	// create model matrix buffer
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(glm::mat4), &bufferModelMatrix);
	// create descriptor set
	vulkanDescriptorSetCreate(device, descriptorSetLayout, &descriptorSet);
	// update descriptor set
	vulkanDescriptorSetUpdateBufferUniform(device, descriptorSet, bufferModelMatrix, 0);
}

// VulkanModel::~VulkanModel
VulkanModel::~VulkanModel()
{
	// destroy descriptor set
	vulkanDescriptorSetDestroy(device, descriptorSet);
	// destroy model matrix buffer
	vulkanBufferDestroy(device, bufferModelMatrix);
}

// VulkanModel::beforeRender
void VulkanModel::beforeRender(VulkanCommandBuffer& commandBuffer)
{
	// update scene descriptor set
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferModelMatrix.buffer, 0, sizeof(matModel), &matModel);
}

// VulkanModel::draw
void VulkanModel::render(VulkanCommandBuffer& commandBuffer)
{
	// bind descriptor set
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.pipelineLayout, 1, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);

	// draw meshes
	if (visible) for (auto& mesh : meshes) mesh->draw(commandBuffer);

	// draw draw debug
	if (visibleDebug) for (auto& mesh : meshesDebug) mesh->draw(commandBuffer);
}

// VulkanModel::afterRender
void VulkanModel::afterRender(VulkanCommandBuffer& commandBuffer)
{
}
