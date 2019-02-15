#pragma once

#include <tiny_obj_loader.h>
#include "vulkan_meshes.hpp"

void loadImageFromFile(
	VulkanDevice& device,
	VulkanImage&  image,
	const char*   fileName);

void loadMesh_obj(
	VulkanDevice&             device,
	VulkanPipeline&           pipeline,
	VulkanSampler&            sampler,
	const char*               fileName,
	const char*               baseDir,
	std::vector<VulkanMesh*>* meshes);
