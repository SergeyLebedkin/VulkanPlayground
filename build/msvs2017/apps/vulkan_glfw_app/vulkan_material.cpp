#include "vulkan_material.hpp"
#include "vulkan_context.hpp"

// VulkamMaterial::VulkamMaterial
VulkanMaterial::VulkanMaterial(VulkanContext& context) : context(context)
{
	// create descriptor set
	vulkanDescriptorSetCreate(context.device, context.descriptorSetLayout_material, &descriptorSet);
}

// VulkamMaterial::~VulkamMaterial
VulkanMaterial::~VulkanMaterial() {
	// destroy descriptor set
	vulkanDescriptorSetDestroy(context.device, descriptorSet);
}

// VulkamMaterial::bind
void VulkanMaterial::bind(VulkanCommandBuffer & commandBuffer) {
	// bind material
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipelineLayout.pipelineLayout, 0, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
}

// VulkamMaterial::setImage
void VulkanMaterial::setImage(VulkanImage& image, VulkanSampler& sampler, uint32_t binding) {
	// update description set
	vulkanDescriptorSetUpdateImage(context.device, descriptorSet, image, sampler, binding);
}
