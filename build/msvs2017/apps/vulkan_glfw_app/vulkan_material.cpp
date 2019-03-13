#include "vulkan_material.hpp"
#include "vulkan_context.hpp"

// VulkamMaterial::VulkamMaterial
VulkanMaterial::VulkanMaterial(VulkanContext& context) : VulkanContextObject(context) {
	// create descriptor set
	vulkanDescriptorSetCreate(context.device, context.descriptorSetLayout_material, &descriptorSet);
	// create buffers
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(materialInfo), &bufferMaterialColors);
	// set descriptor set (binding 1)
	vulkanDescriptorSetUpdateBufferUniform(context.device, descriptorSet, bufferMaterialColors, 1);
}

// VulkamMaterial::~VulkamMaterial
VulkanMaterial::~VulkanMaterial() {
	// create buffers
	vulkanBufferDestroy(context.device, bufferMaterialColors);
	// destroy descriptor set
	vulkanDescriptorSetDestroy(context.device, descriptorSet);
}

// VulkanMaterial::setDiffuseColor
void VulkanMaterial::setDiffuseColor(const glm::vec4 color) {
	materialInfo.diffuseColor = color;
}

// VulkanMaterial::setAmbientColor
void VulkanMaterial::setAmbientColor(const glm::vec4 color) {
	materialInfo.ambientColor = color;
}

// VulkanMaterial::setEmissionColor
void VulkanMaterial::setEmissionColor(const glm::vec4 color) {
	materialInfo.emissionColor = color;
}

// VulkanMaterial::setSpecularColor
void VulkanMaterial::setSpecularColor(const glm::vec4 color) {
	materialInfo.specularColor = color;
}

// VulkanMaterial::setSpecularFactor
void VulkanMaterial::setSpecularFactor(float factor) {
	materialInfo.specularFactor = factor;
}

// VulkanMaterial::getDiffuseColor
glm::vec4 VulkanMaterial::getDiffuseColor() const {
	return materialInfo.diffuseColor;
}

// VulkanMaterial::getAmbientColor
glm::vec4 VulkanMaterial::getAmbientColor() const {
	return materialInfo.ambientColor;
}

// VulkanMaterial::getEmissionColor
glm::vec4 VulkanMaterial::getEmissionColor() const {
	return materialInfo.emissionColor;
}

// VulkanMaterial::getSpecularColor
glm::vec4 VulkanMaterial::getSpecularColor() const {
	return materialInfo.specularColor;
}

// VulkanMaterial::getSpecularFactor
float VulkanMaterial::getSpecularFactor() const {
	return materialInfo.specularFactor;
}

// VulkanMaterial::setDiffuseImage
void VulkanMaterial::setDiffuseImage(VulkanImage* image, VulkanSampler* sampler) {
	// set diffuse image to descriptor set (binding 0)
	vulkanDescriptorSetUpdateImage(context.device, descriptorSet, *image, *sampler, 0);
}

// VulkanMaterial::setDiffuseImage
void VulkanMaterial::setNormalMapImage(VulkanImage* image, VulkanSampler* sampler) {
	// set diffuse image to descriptor set (binding 2)
	vulkanDescriptorSetUpdateImage(context.device, descriptorSet, *image, *sampler, 2);
}

// VulkanMaterial::update
void VulkanMaterial::update(VulkanCommandBuffer& commandBuffer) {
	// update material color buffers
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferMaterialColors.buffer, 0, sizeof(materialInfo), &materialInfo);
}

// VulkanMaterial::bind
void VulkanMaterial::bind(VulkanCommandBuffer& commandBuffer) {
	// bind descriptor set
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		context.pipelineLayout.pipelineLayout, 0, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
};
