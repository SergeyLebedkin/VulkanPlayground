#pragma once

#include <vktoolkit.hpp>
#include <tiny_obj_loader.h>

void loadImageFromFile(
	VulkanDevice& device,
	VulkanImage&  image,
	const char*   fileName);

void loadMesh_obj(
	VulkanDevice& device,
	VulkanBuffer& bufferPos,
	VulkanBuffer& bufferTex,
	VulkanBuffer& bufferNrm,
	uint32_t&     vertexCount,
	const char*   fileName,
	const char*   baseDir);
