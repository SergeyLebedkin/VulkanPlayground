#include "vulkan_material.hpp"

// VulkamMaterial::VulkamMaterial
VulkanMaterial::VulkanMaterial(
	VulkanDevice&              device,
	VulkanPipelineLayout&      pipelineLayout,
	VulkanDescriptorSetLayout& descriptorSetLayout) :
	device(device), pipelineLayout(pipelineLayout)
{
	// create descroptor set
	vulkanDescriptorSetCreate(device, descriptorSetLayout, &descriptorSet);
}

// VulkamMaterial::~VulkamMaterial
VulkanMaterial::~VulkanMaterial() {
	// destroy descroptor set
	vulkanDescriptorSetDestroy(device, descriptorSet);
}

// VulkamMaterial::bind
void VulkanMaterial::bind(VulkanCommandBuffer & commandBuffer) {
	// bind material
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.pipelineLayout, 0, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
}

// VulkamMaterial::setImage
void VulkanMaterial::setImage(VulkanImage& image, VulkanSampler& sampler, uint32_t binding) {
	// uodate description set
	vulkanDescriptorSetUpdateImage(device, descriptorSet, image, sampler, binding);
}
