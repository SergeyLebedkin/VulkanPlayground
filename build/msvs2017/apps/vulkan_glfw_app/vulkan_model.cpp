#include "vulkan_model.hpp"

// VulkanModel::VulkanModel
VulkanModel::VulkanModel(
	VulkanDevice&              device,
	VulkanDescriptorSetLayout& descriptorSetLayout) :
	device(device),
	visible(VK_TRUE),
	visibleDebug(VK_FALSE)
{
	// create view-proj matrices buffer
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(glm::mat4), &bufferModelMatrix);
	// create descroptor set
	vulkanDescriptorSetCreate(device, descriptorSetLayout, &descriptorSet);
	// update descroptor set
	vulkanDescriptorSetUpdateBufferUniform(device, descriptorSet, bufferModelMatrix, 0);
}

// VulkanModel::~VulkanModel
VulkanModel::~VulkanModel()
{
	// destroy descroptor set
	vulkanDescriptorSetDestroy(device, descriptorSet);
	// destroy model matrix buffer
	vulkanBufferDestroy(device, bufferModelMatrix);
}

// VulkanModel::draw
void VulkanModel::draw(VulkanCommandBuffer& commandBuffer)
{
	if (visible)
	{
		// update scene descriptor set
		vulkanBufferWrite(device, bufferModelMatrix, 0, sizeof(matModel), &matModel);

		// bind descroptor set
		//vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 1, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);

		// draw meshes
		for (auto& mesh : meshes)
			mesh->draw(commandBuffer);
	}
}
