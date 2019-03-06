#include "vulkan_model.hpp"
#include "vulkan_context.hpp"

// VulkanModel::VulkanModel
VulkanModel::VulkanModel(VulkanContext& context) :
	context(context), visible(VK_TRUE), visibleDebug(VK_FALSE), matModel(glm::mat4(1.0f))
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

// VulkanModel::beforeRender
void VulkanModel::beforeRender(VulkanCommandBuffer& commandBuffer)
{
	// update scene descriptor set
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferModelMatrix.buffer, 0, sizeof(matModel), &matModel);
}

// VulkanModel::draw
void VulkanModel::bind(VulkanCommandBuffer& commandBuffer)
{
	// bind descriptor set
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipelineLayout.pipelineLayout, 1, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
}

// VulkanModel::afterRender
void VulkanModel::afterRender(VulkanCommandBuffer& commandBuffer)
{
	// nothing
}
