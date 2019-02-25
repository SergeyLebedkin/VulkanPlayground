#pragma once

#include <tiny_obj_loader.h>
#include "vulkan_renderer.hpp"
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
	VulkanRender&                 renderer,
	std::string                   fileName,
	std::string                   baseDir,
	std::vector<VulkanMesh*>*     meshes,
	std::vector<VulkanMesh*>*     meshesDebug,
	std::vector<VulkanImage*>*    images,
	std::vector<VulkanMaterial*>* materialList);
