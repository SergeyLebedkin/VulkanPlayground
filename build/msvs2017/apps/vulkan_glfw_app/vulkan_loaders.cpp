#include "vulkan_loaders.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable: 4996)
#include <stb_image.h>
#include <stb_image_write.h>
#pragma warning(pop)

// loadImageFromFile
void loadImageFromFile(
	VulkanDevice& device,
	VulkanImage&  image,
	const char*   fileName)
{
	int width = 0, height = 0, channels = 0;
	stbi_uc* texData = stbi_load(fileName, &width, &height, &channels, 4);
	vulkanImageCreate(device, VK_FORMAT_R8G8B8A8_UNORM, width, height, 1, &image);
	vulkanImageWrite(device, image, 0, texData);
	vulkanImageBuildMipmaps(device, image);
	stbi_image_free(texData);
}

// loadMesh_obj
void loadMesh_obj(
	VulkanDevice&             device,
	VulkanPipeline&           pipeline,
	VulkanSampler&            sampler,
	const char*               fileName,
	const char*               baseDir,
	std::vector<VulkanMesh*>* meshes)
{
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warm, err;
	tinyobj::LoadObj(&attribs, &shapes, &materials, &warm, &err, fileName, baseDir, true);
	assert(shapes.size() > 0);

	// allocate buffers
	std::vector<float> vectorPos{};
	std::vector<float> vectorNrm{};
	std::vector<float> vectorTex{};
	for (const auto& shape : shapes)
	{
		vectorPos.clear();
		vectorNrm.clear();
		vectorTex.clear();
		vectorPos.reserve(shape.mesh.indices.size() * 3);
		vectorNrm.reserve(shape.mesh.indices.size() * 3);
		vectorTex.reserve(shape.mesh.indices.size() * 2);

		// create buffers
		for (tinyobj::index_t index : shape.mesh.indices)
		{
			// if vertex exists
			if (index.vertex_index >= 0)
			{
				vectorPos.push_back(attribs.vertices[3 * index.vertex_index + 0]);
				vectorPos.push_back(attribs.vertices[3 * index.vertex_index + 1]);
				vectorPos.push_back(attribs.vertices[3 * index.vertex_index + 2]);
			}

			// if normal exists
			if (index.normal_index >= 0)
			{
				vectorNrm.push_back(attribs.normals[3 * index.normal_index + 0]);
				vectorNrm.push_back(attribs.normals[3 * index.normal_index + 1]);
				vectorNrm.push_back(attribs.normals[3 * index.normal_index + 2]);
			}

			// if texCoords exists
			if (index.texcoord_index >= 0)
			{
				vectorTex.push_back(attribs.texcoords[2 * index.texcoord_index + 0]);
				vectorTex.push_back(attribs.texcoords[2 * index.texcoord_index + 1]);
			}
		}

		// create mesh
		VulkanMesh_obj* mesh = new VulkanMesh_obj(device, pipeline, sampler);
		vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorPos.size() * sizeof(float), &mesh->bufferPos);
		vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorTex.size() * sizeof(float), &mesh->bufferTex);
		vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorNrm.size() * sizeof(float), &mesh->bufferNrm);
		vulkanBufferWrite(device, mesh->bufferPos, 0, vectorPos.size() * sizeof(float), vectorPos.data());
		vulkanBufferWrite(device, mesh->bufferTex, 0, vectorTex.size() * sizeof(float), vectorTex.data());
		vulkanBufferWrite(device, mesh->bufferNrm, 0, vectorNrm.size() * sizeof(float), vectorNrm.data());
		mesh->vertexCount = (uint32_t)vectorPos.size() / 3;
		meshes->push_back(mesh);
	}
}