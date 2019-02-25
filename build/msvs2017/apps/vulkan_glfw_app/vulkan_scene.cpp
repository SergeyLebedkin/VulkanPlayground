#include "vulkan_scene.hpp"

// VulkanScene::VulkanScene
VulkanScene::VulkanScene(
	VulkanDevice&              device,
	VulkanPipelineLayout&      pipelineLayout,
	VulkanDescriptorSetLayout& descriptorSetLayout) :
	device(device), pipelineLayout(pipelineLayout)
{
	// create view-proj matrices buffer
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 2*sizeof(glm::mat4), &bufferViewProjMatrices);
	// create descroptor set
	vulkanDescriptorSetCreate(device, descriptorSetLayout, &descriptorSet);
	// update descroptor set
	vulkanDescriptorSetUpdateBufferUniform(device, descriptorSet, bufferViewProjMatrices, 0);
}

// VulkanModel::~VulkanModel
VulkanScene::~VulkanScene()
{
	// destroy descroptor set
	vulkanDescriptorSetDestroy(device, descriptorSet);
	// destroy model matrix buffer
	vulkanBufferDestroy(device, bufferViewProjMatrices);
}

// VulkanModel::draw
void VulkanScene::draw(VulkanCommandBuffer& commandBuffer)
{
	// update scene descriptor set
	struct { glm::mat4 matV; glm::mat4 matP; } matrixes{ matView, matProj };
	vulkanBufferWrite(device, bufferViewProjMatrices, 0, sizeof(matrixes), &matrixes);

	// bind descroptor set
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.pipelineLayout, 2, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);

	// draw models
	for (auto& model : models) {
		model->draw(commandBuffer);
	}
}
