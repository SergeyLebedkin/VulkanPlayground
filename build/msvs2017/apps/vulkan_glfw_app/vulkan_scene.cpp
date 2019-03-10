#include "vulkan_scene.hpp"
#include "vulkan_context.hpp"

// VulkanScene::VulkanScene
VulkanScene::VulkanScene(VulkanContext& context) : VulkanContextObject(context)
{
	// create view-projection matrices buffer
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 2*sizeof(glm::mat4), &bufferViewProjectionMatrices);
	// create descriptor set
	vulkanDescriptorSetCreate(context.device, context.descriptorSetLayout_scene, &descriptorSet);
	// update descriptor set
	vulkanDescriptorSetUpdateBufferUniform(context.device, descriptorSet, bufferViewProjectionMatrices, 0);
}

// VulkanModel::~VulkanModel
VulkanScene::~VulkanScene()
{
	// destroy descriptor set
	vulkanDescriptorSetDestroy(context.device, descriptorSet);
	// destroy model matrix buffer
	vulkanBufferDestroy(context.device, bufferViewProjectionMatrices);
}

// VulkanScene::update
void VulkanScene::update(VulkanCommandBuffer& commandBuffer)
{
	// update scene buffers
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferViewProjectionMatrices.buffer, sizeof(glm::mat4) * 0, sizeof(matrixView), &matrixView);
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferViewProjectionMatrices.buffer, sizeof(glm::mat4) * 1, sizeof(matrixProjection), &matrixProjection);

	// update models
	for (auto& model : models)
		model->update(commandBuffer);
}

// VulkanScene::bind
void VulkanScene::bind(VulkanCommandBuffer& commandBuffer)
{
	// bind descriptor set
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipelineLayout.pipelineLayout, 2, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
}
