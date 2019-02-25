#pragma once
#include "vulkan_pipelines.hpp"
      
// VulkanMaterial
class VulkanMaterial {
protected:
	VulkanDevice&         device;
	VulkanPipelineLayout& pipelineLayout;
protected:
	VulkanDescriptorSet descriptorSet{};
public:
	VulkanMaterial(
		VulkanDevice&              device,
		VulkanPipelineLayout&      pipelineLayout,
		VulkanDescriptorSetLayout& descriptorSetLayout);
	virtual ~VulkanMaterial();
	virtual void bind(VulkanCommandBuffer& commandBuffer);
	void setImage(VulkanImage& image, VulkanSampler& sampler, uint32_t binding);
};
