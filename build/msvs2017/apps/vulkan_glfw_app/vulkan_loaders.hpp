#pragma once

#include <vktoolkit.hpp>
#include <iostream>

void createImageProcedural(
	VulkanDevice& device,
	uint32_t      width,
	uint32_t      height,
	VulkanImage&  image);

void loadImageFromFile(
	VulkanDevice& device,
	VulkanImage&  image,
	std::string   fileName);
