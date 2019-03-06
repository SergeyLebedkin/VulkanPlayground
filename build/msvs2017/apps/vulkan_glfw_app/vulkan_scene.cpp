#include "vulkan_scene.hpp"
#include "vulkan_context.hpp"

// VulkanScene::VulkanScene
VulkanScene::VulkanScene(VulkanContext& context) : context(context)
{
	// create view-projection matrices buffer
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 2*sizeof(glm::mat4), &bufferViewProjMatrices);
	// create descriptor set
	vulkanDescriptorSetCreate(context.device, context.descriptorSetLayout_scene, &descriptorSet);
	// update descriptor set
	vulkanDescriptorSetUpdateBufferUniform(context.device, descriptorSet, bufferViewProjMatrices, 0);
}

// VulkanModel::~VulkanModel
VulkanScene::~VulkanScene()
{
	// destroy descriptor set
	vulkanDescriptorSetDestroy(context.device, descriptorSet);
	// destroy model matrix buffer
	vulkanBufferDestroy(context.device, bufferViewProjMatrices);
}

// VulkanScene::beforeRender
void VulkanScene::beforeRender(VulkanCommandBuffer& commandBuffer)
{
	// update scene buffers
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferViewProjMatrices.buffer, sizeof(glm::mat4) * 0, sizeof(matView), &matView);
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferViewProjMatrices.buffer, sizeof(glm::mat4) * 1, sizeof(matProj), &matProj);

	// update models
	for (auto& model : models)
		model->beforeRender(commandBuffer);
}

// VulkanScene::bind
void VulkanScene::bind(VulkanCommandBuffer& commandBuffer)
{
	// bind descriptor set
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipelineLayout.pipelineLayout, 2, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
}

// VulkanScene::afterRender
void VulkanScene::afterRender(VulkanCommandBuffer& commandBuffer)
{
	// update models
	for (auto& model : models)
		model->afterRender(commandBuffer);
}