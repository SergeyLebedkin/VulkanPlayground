#pragma once

#include <tiny_obj_loader.h>
#include "vulkan_meshes.hpp"

void loadImageFromFile(
	VulkanDevice& device,
	VulkanImage&  image,
	std::string   fileName);

void loadMesh_obj(
	VulkanDevice&              device,
	VulkanPipeline&            pipeline,
	VulkanSampler&             sampler,
	std::string                fileName,
	std::string                baseDir,
	std::vector<VulkanMesh*>*  meshes,
	std::vector<VulkanImage*>* images);
