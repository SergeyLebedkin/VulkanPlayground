#include "vulkan_loaders.hpp"
#include <algorithm>

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
	std::string   fileName)
{
	int width = 0, height = 0, channels = 0;
	stbi_uc* texData = stbi_load(fileName.data(), &width, &height, &channels, 4);
	vulkanImageCreate(device, VK_FORMAT_R8G8B8A8_UNORM, width, height, 1, &image);
	vulkanImageWrite(device, image, 0, texData);
	vulkanImageBuildMipmaps(device, image);
	stbi_image_free(texData);
}

// loadMesh_obj
void loadMesh_obj(
	VulkanDevice&              device,
	VulkanPipeline&            pipeline,
	VulkanSampler&             sampler,
	std::string                fileName,
	std::string                baseDir,
	std::vector<VulkanMesh*>*  meshes,
	std::vector<VulkanImage*>* images)
{
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warm, err;
	tinyobj::LoadObj(&attribs, &shapes, &materials, &warm, &err, fileName.data(), baseDir.data(), true);
	assert(shapes.size() > 0);

	// load materials
	for (const auto& material : materials)
	{
		if (material.diffuse_texname.size() > 0)
		{
			auto image = new VulkanImage();
			loadImageFromFile(device, *image, baseDir + material.diffuse_texname);
			images->push_back(image);
		}
	}

	// find min and max
	float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
	float maxX = FLT_MIN, maxY = FLT_MIN, maxZ = FLT_MIN;
	for (const auto& shape : shapes)
	{
		for (tinyobj::index_t index : shape.mesh.indices) 
		{
			maxX = std::max(maxX, attribs.vertices[3 * index.vertex_index + 0]);
			maxY = std::max(maxY, attribs.vertices[3 * index.vertex_index + 1]);
			maxZ = std::max(maxZ, attribs.vertices[3 * index.vertex_index + 2]);
			minX = std::min(minX, attribs.vertices[3 * index.vertex_index + 0]);
			minY = std::min(minY, attribs.vertices[3 * index.vertex_index + 1]);
			minZ = std::min(minZ, attribs.vertices[3 * index.vertex_index + 2]);
		}
	}
	float centerX = (maxX + minX)*0.5f, centerY = (maxY + minY)*0.5f, centerZ = (maxZ + minZ)*0.5f;
	float scale = 1.0f / (std::max(std::max(maxX - minX, maxY - minY), maxZ - minZ)*0.5f);

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
				vectorPos.push_back((attribs.vertices[3 * index.vertex_index + 0] - centerX) * scale);
				vectorPos.push_back((attribs.vertices[3 * index.vertex_index + 1] - centerY) * scale);
				vectorPos.push_back((attribs.vertices[3 * index.vertex_index + 2] - centerZ) * scale);
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
		VulkanMesh_obj* mesh = new VulkanMesh_obj(device);
		vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 16 * sizeof(float), &mesh->bufferMVP);
		vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorPos.size() * sizeof(float), &mesh->bufferPos);
		vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorTex.size() * sizeof(float), &mesh->bufferTex);
		vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorNrm.size() * sizeof(float), &mesh->bufferNrm);
		vulkanBufferWrite(device, mesh->bufferPos, 0, vectorPos.size() * sizeof(float), vectorPos.data());
		vulkanBufferWrite(device, mesh->bufferTex, 0, vectorTex.size() * sizeof(float), vectorTex.data());
		vulkanBufferWrite(device, mesh->bufferNrm, 0, vectorNrm.size() * sizeof(float), vectorNrm.data());
		VulkanDescriptorSetCreate(device, pipeline, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_T1_U1), descriptorSetLayoutBindings_T1_U1, &mesh->descriptorSet);
		if (shape.mesh.material_ids[0] >= 0)
			vulkanDescriptorSetUpdateImage(device, mesh->descriptorSet, *(*images)[shape.mesh.material_ids[0]], sampler, 0);
		vulkanDescriptorSetUpdateBufferUniform(device, mesh->descriptorSet, mesh->bufferMVP, 1);
		mesh->vertexCount = (uint32_t)vectorPos.size() / 3;
		meshes->push_back(mesh);
	}
}