#include "vulkan_scene.hpp"

// VulkanScene::VulkanScene
VulkanScene::VulkanScene(
	VulkanDevice&              device,
	VulkanPipelineLayout&      pipelineLayout,
	VulkanDescriptorSetLayout& descriptorSetLayout) :
	device(device), pipelineLayout(pipelineLayout)
{
	// create view-projection matrices buffer
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 2*sizeof(glm::mat4), &bufferViewProjMatrices);
	// create descriptor set
	vulkanDescriptorSetCreate(device, descriptorSetLayout, &descriptorSet);
	// update descriptor set
	vulkanDescriptorSetUpdateBufferUniform(device, descriptorSet, bufferViewProjMatrices, 0);
}

// VulkanModel::~VulkanModel
VulkanScene::~VulkanScene()
{
	// destroy descriptor set
	vulkanDescriptorSetDestroy(device, descriptorSet);
	// destroy model matrix buffer
	vulkanBufferDestroy(device, bufferViewProjMatrices);
}

// VulkanScene::beforeRender
void VulkanScene::beforeRender(VulkanCommandBuffer& commandBuffer)
{
	// update scene buffers
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferViewProjMatrices.buffer, sizeof(glm::mat4) * 0, sizeof(matView), &matView);
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferViewProjMatrices.buffer, sizeof(glm::mat4) * 1, sizeof(matProj), &matProj);

	// draw models
	for (auto& model : models) {
		model->beforeRender(commandBuffer);
	}
}

// VulkanModel::render
void VulkanScene::render(VulkanCommandBuffer& commandBuffer)
{
	// bind descriptor set
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.pipelineLayout, 2, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);

	// draw models
	for (auto& model : models) {
		model->render(commandBuffer);
	}
}

// VulkanScene::afterRender
void VulkanScene::afterRender(VulkanCommandBuffer& commandBuffer)
{
	// draw models
	for (auto& model : models) {
		model->afterRender(commandBuffer);
	}
}