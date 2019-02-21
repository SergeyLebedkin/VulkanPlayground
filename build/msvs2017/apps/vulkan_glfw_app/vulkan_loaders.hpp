#pragma once

#include <tiny_obj_loader.h>
#include "vulkan_meshes.hpp"

void createImageProcedural(
	VulkanDevice& device,
	uint32_t      width,
	uint32_t      height,
	VulkanImage&  image);

void loadImageFromFile(
	VulkanDevice& device,
	VulkanImage&  image,
	std::string   fileName);

void loadMesh_obj(
	VulkanDevice&              device,
	VulkanShader&              shader,
	VulkanShader&              shaderLines,
	VulkanSampler&             sampler,
	VulkanImage&               imageDefault,
	std::string                fileName,
	std::string                baseDir,
	std::vector<VulkanMesh*>*  meshes,
	std::vector<VulkanMesh*>*  meshesLines,
	std::vector<VulkanImage*>* images);
