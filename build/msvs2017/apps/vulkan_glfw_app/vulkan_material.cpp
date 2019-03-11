#include "vulkan_material.hpp"
#include "vulkan_context.hpp"

// VulkamMaterial::VulkamMaterial
VulkanMaterial::VulkanMaterial(VulkanContext& context) : VulkanContextObject(context) {
	// create descriptor set
	vulkanDescriptorSetCreate(context.device, context.descriptorSetLayout_material, &descriptorSet);
}

// VulkamMaterial::~VulkamMaterial
VulkanMaterial::~VulkanMaterial() {
	// destroy descriptor set
	vulkanDescriptorSetDestroy(context.device, descriptorSet);
}

// VulkanMaterial::bind
void VulkanMaterial::bind(VulkanCommandBuffer& commandBuffer) {
	// bind descriptor set
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		context.pipelineLayout.pipelineLayout, 0, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
};

//////////////////////////////////////////////////////////////////////////

// VulkanMaterial_textured::setDiffuseImage
void VulkanMaterial_textured::setDiffuseImage(VulkanImage* image, VulkanSampler* sampler) {
	// set diffuse image to descriptor set (binding 0)
	vulkanDescriptorSetUpdateImage(context.device, descriptorSet, *image, *sampler, 0);
}

//////////////////////////////////////////////////////////////////////////

// VulkanMaterial_colored::VulkanMaterial_colored
VulkanMaterial_colored::VulkanMaterial_colored(VulkanContext& context) : VulkanMaterial_textured(context) {
	// create buffers
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(materialInfo), &bufferMaterialColors);
	// set descriptor set (binding 1)
	vulkanDescriptorSetUpdateBufferUniform(context.device, descriptorSet, bufferMaterialColors, 1);
};

// VulkanMaterial_colored::VulkanMaterial_colored
VulkanMaterial_colored::~VulkanMaterial_colored() {
	// create buffers
	vulkanBufferDestroy(context.device, bufferMaterialColors);
};

// VulkanMaterial_colored::setDiffuseColor
void VulkanMaterial_colored::setDiffuseColor(const glm::vec4 color) {
	materialInfo.diffuseColor = color;
}

// VulkanMaterial_colored::setAmbientColor
void VulkanMaterial_colored::setAmbientColor(const glm::vec4 color) {
	materialInfo.ambientColor = color;
}

// VulkanMaterial_colored::setEmissionColor
void VulkanMaterial_colored::setEmissionColor(const glm::vec4 color) {
	materialInfo.emissionColor = color;
}

// VulkanMaterial_colored::setSpecularColor
void VulkanMaterial_colored::setSpecularColor(const glm::vec4 color) {
	materialInfo.specularColor = color;
}

// VulkanMaterial_colored::setSpecularFactor
void VulkanMaterial_colored::setSpecularFactor(float factor) {
	materialInfo.specularFactor = factor;
}

// VulkanMaterial_colored::getDiffuseColor
glm::vec4 VulkanMaterial_colored::getDiffuseColor() const {
	return materialInfo.diffuseColor;
}

// VulkanMaterial_colored::getAmbientColor
glm::vec4 VulkanMaterial_colored::getAmbientColor() const {
	return materialInfo.ambientColor;
}

// VulkanMaterial_colored::getEmissionColor
glm::vec4 VulkanMaterial_colored::getEmissionColor() const {
	return materialInfo.emissionColor;
}

// VulkanMaterial_colored::getSpecularColor
glm::vec4 VulkanMaterial_colored::getSpecularColor() const {
	return materialInfo.specularColor;
}

// VulkanMaterial_colored::getSpecularFactor
float VulkanMaterial_colored::getSpecularFactor() const {
	return materialInfo.specularFactor;
}

// VulkanMaterial_colored::update
void VulkanMaterial_colored::update(VulkanCommandBuffer& commandBuffer) {
	// update material color buffers
	vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferMaterialColors.buffer, 0, sizeof(materialInfo), &materialInfo);
};