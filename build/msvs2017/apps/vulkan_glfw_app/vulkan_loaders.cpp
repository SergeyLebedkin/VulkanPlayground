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

// loadMesh_obj
void loadMesh_obj(
	VulkanRender&                 renderer,
	std::string                   fileName,
	std::string                   baseDir,
	std::vector<VulkanMesh*>*     meshes,
	std::vector<VulkanMesh*>*     meshesDebug,
	std::vector<VulkanImage*>*    images,
	std::vector<VulkanMaterial*>* materialList)
{
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warm, err;
	tinyobj::LoadObj(&attribs, &shapes, &materials, &warm, &err, fileName.data(), baseDir.data(), true);
	assert(shapes.size() > 0);

	// load materials
	for (const auto& material : materials) {
		if (material.diffuse_texname.size() > 0) {
			// create image
			VulkanImage* image = new VulkanImage();
			loadImageFromFile(renderer.device, *image, baseDir + material.diffuse_texname);
			images->push_back(image);

			// create material
			VulkanMaterial* material = new VulkanMaterial(
				renderer.device,
				renderer.pipelineLayout,
				renderer.descriptorSetLayout_material);
			material->setImage(*image, renderer.samplerDefault, 0);
			materialList->push_back(material);
		}
	}

	// find min and max
	glm::vec3 maxPos = glm::vec3(FLT_MIN);
	glm::vec3 minPos = glm::vec3(FLT_MAX);
	for (const auto& shape : shapes)
	{
		for (tinyobj::index_t index : shape.mesh.indices) 
		{	
			maxPos.x = std::max(maxPos.x, attribs.vertices[3 * index.vertex_index + 0]);
			maxPos.y = std::max(maxPos.y, attribs.vertices[3 * index.vertex_index + 1]);
			maxPos.z = std::max(maxPos.z, attribs.vertices[3 * index.vertex_index + 2]);
			minPos.x = std::min(minPos.x, attribs.vertices[3 * index.vertex_index + 0]);
			minPos.y = std::min(minPos.y, attribs.vertices[3 * index.vertex_index + 1]);
			minPos.z = std::min(minPos.z, attribs.vertices[3 * index.vertex_index + 2]);
		}
	}
	glm::vec3 centerPos = (maxPos + minPos) * 0.5f;
	glm::vec3 lengthPos = maxPos - minPos;
	float scale = 1.0f / (std::max(std::max(lengthPos.x, lengthPos.y), lengthPos.z)*0.5f);

	// allocate buffers mesh
	std::vector<glm::vec3> vectorPos{};
	std::vector<glm::vec3> vectorNrm{};
	std::vector<glm::vec3> vectorTan{};
	std::vector<glm::vec3> vectorBit{};
	std::vector<glm::vec2> vectorTex{};
	// allocate buffers normals
	std::vector<glm::vec3> vectorNrmPos{};
	std::vector<glm::vec4> vectorNrmCol{};
	for (const auto& shape : shapes)
	{
		vectorPos.clear();
		vectorNrm.clear();
		vectorTan.clear();
		vectorBit.clear();
		vectorTex.clear();
		vectorNrmPos.clear();
		vectorNrmCol.clear();
		vectorPos.reserve(shape.mesh.indices.size());
		vectorNrm.reserve(shape.mesh.indices.size());
		vectorTan.reserve(shape.mesh.indices.size());
		vectorBit.reserve(shape.mesh.indices.size());
		vectorTex.reserve(shape.mesh.indices.size());
		vectorNrmPos.reserve(shape.mesh.indices.size() * 2 * 3);
		vectorNrmCol.reserve(shape.mesh.indices.size() * 2 * 3);

		// create buffers
		for (tinyobj::index_t index : shape.mesh.indices)
		{
			// if vertex exists
			if (index.vertex_index >= 0)
				vectorPos.push_back(glm::vec3(
					(attribs.vertices[3 * index.vertex_index + 0] - centerPos.x) * scale,
					(attribs.vertices[3 * index.vertex_index + 1] - centerPos.y) * scale,
					(attribs.vertices[3 * index.vertex_index + 2] - centerPos.z) * scale));
			else vectorPos.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

			// if normal exists
			if (index.normal_index >= 0)
				vectorNrm.push_back(glm::vec3(
					attribs.normals[3 * index.normal_index + 0],
					attribs.normals[3 * index.normal_index + 1],
					attribs.normals[3 * index.normal_index + 2]));
			else vectorNrm.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

			// if texCoords exists
			if (index.texcoord_index >= 0)
				vectorTex.push_back(glm::vec2(
					attribs.texcoords[2 * index.texcoord_index + 0],
					attribs.texcoords[2 * index.texcoord_index + 1]));
			else vectorTex.push_back(glm::vec2(0.0f, 0.0f));
		}

		// calculate bi-normal and tangent
		for (size_t i = 0; i < vectorPos.size(); i += 3) {
			// Shortcuts for vertices
			glm::vec3& v0 = vectorPos[i + 0];
			glm::vec3& v1 = vectorPos[i + 1];
			glm::vec3& v2 = vectorPos[i + 2];

			// Shortcuts for UVs
			glm::vec2& uv0 = vectorTex[i + 0];
			glm::vec2& uv1 = vectorTex[i + 1];
			glm::vec2& uv2 = vectorTex[i + 2];

			// Edges of the triangle : position delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);
			glm::vec3 bitangent = glm::normalize((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r);

			vectorTan.push_back(tangent);
			vectorTan.push_back(tangent);
			vectorTan.push_back(tangent);
			vectorBit.push_back(bitangent);
			vectorBit.push_back(bitangent);
			vectorBit.push_back(bitangent);
		}

		// add normals, tangents and bi-normals
		for (size_t i = 0; i < vectorPos.size(); i++) {
			vectorNrmPos.push_back(vectorPos[i]);
			vectorNrmPos.push_back(vectorPos[i] + vectorNrm[i]*0.05f);
			vectorNrmCol.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			vectorNrmCol.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			vectorNrmPos.push_back(vectorPos[i]);
			vectorNrmPos.push_back(vectorPos[i] + vectorTan[i] * 0.05f);
			vectorNrmCol.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			vectorNrmCol.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

			vectorNrmPos.push_back(vectorPos[i]);
			vectorNrmPos.push_back(vectorPos[i] + vectorBit[i] * 0.05f);
			vectorNrmCol.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
			vectorNrmCol.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		}

		// get image from material
		VulkanMaterial* material = (shape.mesh.material_ids[0] >= 0)
			? materialList->at(shape.mesh.material_ids[0])
			: renderer.materialDefault;

		// create mesh
		if (meshes) {
			VulkanMesh* mesh = new VulkanMesh_obj(
				renderer.device,
				material,
				vectorPos,
				vectorTex,
				vectorNrm);
			meshes->push_back(mesh);
		}

		// create mesh lines
		if (meshesDebug) {
			VulkanMesh* mesh = new VulkanMesh_lines(
				renderer.device,
				vectorNrmPos,
				vectorNrmCol);
			meshesDebug->push_back(mesh);
		}
	}
}