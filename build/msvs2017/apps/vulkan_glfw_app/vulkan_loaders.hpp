#pragma once

#include <tiny_obj_loader.h>
#include "vulkan_meshes.hpp"

void loadImageFromFile(
	VulkanDevice& device,
	VulkanImage&  image,
	std::string   fileName);

void loadMesh_obj(
	VulkanDevice&              device,
	VulkanShader&              shader,
	VulkanShader&              shaderLines,
	VulkanSampler&             sampler,
	std::string                fileName,
	std::string                baseDir,
	std::vector<VulkanMesh*>*  meshes,
	std::vector<VulkanMesh*>*  meshesLines,
	std::vector<VulkanImage*>* images);
