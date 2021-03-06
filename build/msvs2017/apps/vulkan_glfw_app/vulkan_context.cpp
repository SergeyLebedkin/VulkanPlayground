#include "vulkan_context.hpp"
#include "vulkan_descriptors.hpp"
#include "vulkan_loaders.hpp"

// VulkanContext::VulkanContext
VulkanContext::VulkanContext(
	std::vector<const char *>& enabledInstanceLayerNames,
	std::vector<const char *>& enabledInstanceExtensionNames,
	std::vector<const char *>& enabledDeviceExtensionNames,
	VkPhysicalDeviceFeatures&  physicalDeviceFeatures)
{
	// create instance and device
	vulkanInstanceCreate(enabledInstanceLayerNames, enabledInstanceExtensionNames, &instance);
	vulkanDeviceCreate(instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, physicalDeviceFeatures, enabledDeviceExtensionNames, &device);

	// create descriptor set layouts
	vulkanDescriptorSetLayoutCreate(device, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_material), descriptorSetLayoutBindings_material, &descriptorSetLayout_material);
	vulkanDescriptorSetLayoutCreate(device, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_model), descriptorSetLayoutBindings_model, &descriptorSetLayout_model);
	vulkanDescriptorSetLayoutCreate(device, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_scene), descriptorSetLayoutBindings_scene, &descriptorSetLayout_scene);

	// list of descriptor set layout
	VkDescriptorSetLayout descriptorSetLayouts[] = {
		descriptorSetLayout_material.descriptorSetLayout,
		descriptorSetLayout_model.descriptorSetLayout,
		descriptorSetLayout_scene.descriptorSetLayout,
	};

	// create pipeline layout
	vulkanPipelineLayoutCreate(device, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayouts), descriptorSetLayouts, &pipelineLayout);

	// create default sampler and material
	vulkanSamplerCreate(device, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_TRUE, &defaultSampler);
	createDefaultImage();
}

// VulkanContext::~VulkanContext
VulkanContext::~VulkanContext()
{
	// destroy default material and sampler
	vulkanImageDestroy(device, defaultImage);
	vulkanSamplerDestroy(device, defaultSampler);

	// destroy pipeline layouts
	vulkanPipelineLayoutDestroy(device, pipelineLayout);

	// destroy shaders
	vulkanDescriptorSetLayoutDestroy(device, descriptorSetLayout_scene);
	vulkanDescriptorSetLayoutDestroy(device, descriptorSetLayout_model);
	vulkanDescriptorSetLayoutDestroy(device, descriptorSetLayout_material);

	// destroy device and instance
	vulkanDeviceDestroy(device);
	vulkanInstanceDestroy(instance);
}

// createDefaultImage
void VulkanContext::createDefaultImage()
{
	createImageProcedural(device, 1024, 1024, defaultImage);
}
