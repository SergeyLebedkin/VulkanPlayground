#pragma once
#include "vulkan_descriptors.hpp"

// VulkanContext
class VulkanContext;
      
// VulkanMaterial
class VulkanMaterial {
protected:
	// context
	VulkanContext& context;
protected:
	// descriptor set
	VulkanDescriptorSet descriptorSet{};
public:
	// constructor and destructor
	VulkanMaterial(VulkanContext& context);
	virtual ~VulkanMaterial();

	// bind material
	virtual void bind(VulkanCommandBuffer& commandBuffer);

	// set image
	void setImage(VulkanImage& image, VulkanSampler& sampler, uint32_t binding);
};
