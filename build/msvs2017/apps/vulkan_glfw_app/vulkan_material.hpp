#pragma once
#include "vulkan_descriptors.hpp"
#include "vulkan_context.hpp"
#include <glm/vec4.hpp>

// VulkanMaterial
class VulkanMaterial : public VulkanContextObject {
protected:
	VulkanDescriptorSet descriptorSet{};
public:
	// constructor and destructor
	VulkanMaterial(VulkanContext& context);
	~VulkanMaterial();

	// bind
	virtual void bind(VulkanCommandBuffer& commandBuffer);
};

// VulkanMaterial_textured
class VulkanMaterial_textured : public VulkanMaterial {
public:
	// constructor and destructor
	VulkanMaterial_textured(VulkanContext& context) : VulkanMaterial(context) {};

	// set diffuse image
	void setDiffuseImage(VulkanImage* image, VulkanSampler* sampler);
};

// VulkanMaterialInfo
struct VulkanMaterialInfo {
	glm::vec4 diffuseColor = glm::vec4(1.0f);
	glm::vec4 ambientColor = glm::vec4(1.0f);
	glm::vec4 emissionColor = glm::vec4(1.0f);
	glm::vec4 specularColor = glm::vec4(1.0f);
	float specularFactor = 32.0f;
};

// VulkanMaterial_colored
class VulkanMaterial_colored : public VulkanMaterial_textured {
protected:
	// buffer material colors
	VulkanBuffer bufferMaterialColors{};
protected:
	// material info
	VulkanMaterialInfo materialInfo{};
public:
	// constructor and destructor
	VulkanMaterial_colored(VulkanContext& context);
	~VulkanMaterial_colored();

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

	// update
	void update(VulkanCommandBuffer& commandBuffer);
};

