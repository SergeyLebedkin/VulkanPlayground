#pragma once
#include <vktoolkit.hpp>

// VulkanContext
class VulkanContext {
public:
	// instance and device
	VulkanInstance instance{};
	VulkanDevice   device{};
public:
	// descriptor set layouts
	VulkanDescriptorSetLayout descriptorSetLayout_material{};
	VulkanDescriptorSetLayout descriptorSetLayout_model{};
	VulkanDescriptorSetLayout descriptorSetLayout_scene{};
	// pipeline layout
	VulkanPipelineLayout pipelineLayout{};
public:
	VulkanImage   defaultImage{};
	VulkanSampler defaultSampler{};
private:
	void createDefaultImage();
public:
	// constructor and destructor
	VulkanContext(
		std::vector<const char *>& enabledInstanceLayerNames,
		std::vector<const char *>& enabledInstanceExtensionNames,
		std::vector<const char *>& enabledDeviceExtensionNames,
		VkPhysicalDeviceFeatures&  physicalDeviceFeatures);
	~VulkanContext();
};

// VulkanContextObject
class VulkanContextObject {
protected:
	// parent context
	VulkanContext& context;
public:
	// constructor and destructor
	VulkanContextObject(VulkanContext& context) : context(context) {}
	virtual ~VulkanContextObject() {}

	// update
	virtual void update(VulkanCommandBuffer& commandBuffer) {};
};

// VulkanContextDrawableObject
class VulkanContextDrawableObject : public VulkanContextObject {
public:
	// constructor and destructor
	VulkanContextDrawableObject(VulkanContext& context) : VulkanContextObject(context) {}
	virtual ~VulkanContextDrawableObject() {}

	// draw
	virtual void draw(VulkanCommandBuffer& commandBuffer) {};
};