#include "vulkan_loaders.hpp"
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable: 4996)
#include <stb_image.h>
#include <stb_image_write.h>
#pragma warning(pop)

// createImageProcedural
void createImageProcedural(
	VulkanDevice& device,
	uint32_t      width,
	uint32_t      height,
	VulkanImage&  image)
{
	// create data for image
	struct pixel_u8 { uint8_t r, g, b, a; };
	pixel_u8* texData = new pixel_u8[width * height];

	// create image instance
	vulkanImageCreate(device, VK_FORMAT_R8G8B8A8_UNORM, width, height, 1, &image);

	for (uint32_t mip = 0; mip < image.mipLevels; mip++) {
		memset(texData, mip * (255 / image.mipLevels), width * height * sizeof(pixel_u8));
		vulkanImageWrite(device, image, mip, texData);
	}

	delete[] texData;
}

// loadImageFromFile
void loadImageFromFile(
	VulkanDevice& device,
	VulkanImage&  image,
	std::string   fileName)
{
	// load image data from file
	int width = 0, height = 0, channels = 0;
	stbi_uc* texData = stbi_load(fileName.data(), &width, &height, &channels, 4);

	// create and setup vulkan image
	vulkanImageCreate(device, VK_FORMAT_R8G8B8A8_UNORM, width, height, 1, &image);
	vulkanImageWrite(device, image, 0, texData);
	vulkanImageBuildMipmaps(device, image);

	// free image data
	stbi_image_free(texData);
}