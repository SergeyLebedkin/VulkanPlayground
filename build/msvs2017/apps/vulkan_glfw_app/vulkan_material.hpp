#pragma once
#include "vulkan_descriptors.hpp"
#include "vulkan_context.hpp"
#include <glm/vec4.hpp>

// VulkanMaterialUsage
enum VulkanMaterialUsage {
	VULKAN_MATERIAL_USAGE_COLOR = 0,
	VULKAN_MATERIAL_USAGE_COLOR_LIGHT = 1,
	VULKAN_MATERIAL_USAGE_COLOR_TEXTURE = 2,
	VULKAN_MATERIAL_USAGE_COLOR_TEXTURE_LIGHT = 3,
	VULKAN_MATERIAL_USAGE_COLOR_TEXTURE_LIGHT_BUMPMAP = 4,
	VULKAN_MATERIAL_USAGE_COLOR_TEXTURE_LIGHT_PBR = 5,
	VULKAN_MATERIAL_USAGE_BEGIN_RANGE = VULKAN_MATERIAL_USAGE_COLOR,
	VULKAN_MATERIAL_USAGE_END_RANGE = VULKAN_MATERIAL_USAGE_COLOR_TEXTURE_LIGHT_PBR,
	VULKAN_MATERIAL_USAGE_RANGE_SIZE = (VULKAN_MATERIAL_USAGE_END_RANGE - VULKAN_MATERIAL_USAGE_BEGIN_RANGE + 1),
	VULKAN_MATERIAL_USAGE_MAX_ENUM = 0x7FFFFFFF
};

// VulkanMaterialInfo
struct VulkanMaterialInfo {
	glm::vec4 diffuseColor = glm::vec4(1.0f);
	glm::vec4 ambientColor = glm::vec4(1.0f);
	glm::vec4 emissionColor = glm::vec4(1.0f);
	glm::vec4 specularColor = glm::vec4(1.0f);
	float specularFactor = 32.0f;
};

// VulkanMaterial
class VulkanMaterial : public VulkanContextObject {
protected:
	// descriptor set
	VulkanDescriptorSet descriptorSet{};
	// buffer material colors
	VulkanBuffer bufferMaterialColors{};
	// material info
	VulkanMaterialInfo materialInfo{};
public:
	// constructor and destructor
	VulkanMaterial(VulkanContext& context);
	~VulkanMaterial();

	// set colors
	void setDiffuseColor(const glm::vec4 color);
	void setAmbientColor(const glm::vec4 color);
	void setEmissionColor(const glm::vec4 color);
	void setSpecularColor(const glm::vec4 color);
	void setSpecularFactor(float factor);

	// get colors
	glm::vec4 getDiffuseColor() const;
	glm::vec4 getAmbientColor() const;
	glm::vec4 getEmissionColor() const;
	glm::vec4 getSpecularColor() const;
	float getSpecularFactor() const;

	// set diffuse image
	void setDiffuseImage(VulkanImage* image, VulkanSampler* sampler);
	void setNormalMapImage(VulkanImage* image, VulkanSampler* sampler);

	// update
	void update(VulkanCommandBuffer& commandBuffer) override;

	// bind
	virtual void bind(VulkanCommandBuffer& commandBuffer);
};
