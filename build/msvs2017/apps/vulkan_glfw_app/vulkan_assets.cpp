#include "vulkan_assets.hpp"
#include "vulkan_loaders.hpp"
#include "vulkan_renderer.hpp"
#include <tiny_obj_loader.h>

// vertex array
std::vector<VertexStruct_P4_C4_T2> vertices = {
	{ +1.0f, -1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +1.0f, +1.0, /**/+1, +0 },
	{ +1.0f, +1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +1.0f, +1.0, /**/+1, +1 },
	{ -1.0f, -1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +1.0f, +1.0, /**/+0, +0 },
	{ -1.0f, +1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +1.0f, +1.0, /**/+0, +1 },
};

// index array
uint16_t indexes[] = { 0, 1, 2, 2, 1, 3 };

//////////////////////////////////////////////////////////////////////////

// VulkanAssetManager::VulkanAssetManager
VulkanAssetManager::VulkanAssetManager(VulkanRender* renderer)
	: renderer(renderer)
{
	// check handles
	assert(renderer);

	// create default image
	createImageProcedural(renderer->device, 256, 256, defaultImage);

	// create default material
	defaultMaterial = new VulkanMaterial(
		renderer->device,
		renderer->pipelineLayout,
		renderer->descriptorSetLayout_material);
	defaultMaterial->setImage(defaultImage, renderer->samplerDefault, 0);

	// create default mesh
	defaultMesh = new VulkanMesh_gui(
		renderer->device,
		renderer->pipeline_gui,
		defaultMaterial,
		vertices
	);

	// create default model
	defaultModelAsset = new VulkanModelAsset();
	defaultModelAsset->meshes.push_back(defaultMesh);
};

// VulkanAssetManager::~VulkanAssetManager
VulkanAssetManager::~VulkanAssetManager()
{
	// destroy default handles
	delete defaultModelAsset;
	delete defaultMesh;
	delete defaultMaterial;
	vulkanImageDestroy(renderer->device, defaultImage);

	// destroy handles
	for (auto& modelAsset : mapModelAssets) delete modelAsset.second;
	for (auto& material : mapMaterials) delete material.second;
	for (auto& image : mapImages) vulkanImageDestroy(renderer->device, *image.second);

	// delete meshes
	for (auto& mesh : meshes) delete mesh;
	meshes.clear();

	// clear maps
	mapModelAssets.clear();
	mapMaterials.clear();
	mapImages.clear();
};

// VulkanAssetManager::isExist_Image
bool VulkanAssetManager::isExist_Image(const std::string name) {
	return (mapImages.find(name) != mapImages.end());
}

// VulkanAssetManager::isExist_Material
bool VulkanAssetManager::isExist_Material(const std::string name) {
	return (mapMaterials.find(name) != mapMaterials.end());
}

// VulkanAssetManager::isExist_ModelAsset
bool VulkanAssetManager::isExist_ModelAsset(const std::string name) {
	return (mapModelAssets.find(name) != mapModelAssets.end());
}

// VulkanAssetManager::add_Image
void VulkanAssetManager::add_Image(VulkanImage* image, const std::string name)
{
	assert(image);
	if (!isExist_Image(name)) mapImages[name] = image;
};

// VulkanAssetManager::add_Material
void VulkanAssetManager::add_Material(VulkanMaterial* material, const std::string name)
{
	assert(material);
	if (!isExist_Material(name)) mapMaterials[name] = material;
};

// VulkanAssetManager::add_ModelAsset
void VulkanAssetManager::add_ModelAsset(VulkanModelAsset* modelAsset, const std::string name)
{
	assert(modelAsset);
	if (!isExist_ModelAsset(name)) mapModelAssets[name] = modelAsset;
};

// VulkanAssetManager::getOrDefault_Image
VulkanImage* VulkanAssetManager::getOrDefault_Image(const std::string name) 
{ 
	if (isExist_Image(name)) 
		return mapImages[name];
	return &defaultImage;
};

// VulkanAssetManager::getOrDefault_Material
VulkanMaterial* VulkanAssetManager::getOrDefault_Material(const std::string name)
{ 
	if (isExist_Material(name))
		return mapMaterials[name];
	return defaultMaterial;
};

// VulkanAssetManager::getOrDefault_ModelAsset
VulkanModelAsset* VulkanAssetManager::getOrDefault_ModelAsset(const std::string name)
{ 
	if (isExist_ModelAsset(name))
		return mapModelAssets[name];
	return defaultModelAsset;
}

// VulkanAssetManager::getOrLoadFromFile_Image
VulkanImage* VulkanAssetManager::getOrLoadFromFile_Image(const std::string fileName)
{
	// if already loaded or exists
	if (isExist_Image(fileName))
		return getOrDefault_Image(fileName);

	// load image from file 
	VulkanImage* image = new VulkanImage();
	loadImageFromFile(renderer->device, *image, fileName);
	add_Image(image, fileName);
	return image;
};

// VulkanAssetManager::createOrDefaut_Model
VulkanModel* VulkanAssetManager::createOrDefaut_Model(std::string name) 
{
	// get or default model asset by name
	VulkanModelAsset* modelAsset = getOrDefault_ModelAsset(name);

	// create new model
	VulkanModel* model = new VulkanModel(
		renderer->device, 
		renderer->pipelineLayout, 
		renderer->descriptorSetLayout_model);
	model->meshes.insert(model->meshes.end(), modelAsset->meshes.begin(), modelAsset->meshes.end());
	model->meshesDebug.insert(model->meshesDebug.end(), modelAsset->meshesDebug.begin(), modelAsset->meshesDebug.end());
	return model;
};

// loadFromFileObj
bool VulkanAssetManager::loadFileObj(std::string fileName, std::string basePath)
{
	// return if exists
	if (isExist_ModelAsset(fileName))
		return true;

	// load and parse obj file
	std::string warm, err;
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	tinyobj::LoadObj(&attribs, &shapes, &materials, &warm, &err, fileName.data(), basePath.data(), true);
	assert(shapes.size() > 0);

	// load materials
	for (const auto& material_obj : materials) {
		// check material name
		assert(material_obj.name.size() > 0);

		// create material
		VulkanMaterial* material = new VulkanMaterial(
			renderer->device,
			renderer->pipelineLayout,
			renderer->descriptorSetLayout_material);
		add_Material(material, material_obj.name);

		// load diffuse image
		if (material_obj.diffuse_texname.size() > 0) {
			VulkanImage* image = getOrLoadFromFile_Image(basePath + material_obj.diffuse_texname);
			// add diffuse image to material
			material->setImage(*image, renderer->samplerDefault, 0);
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

	// local meshes
	std::vector<VulkanMesh*> local_meshes{};
	std::vector<VulkanMesh*> local_meshes_debug{};

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
		// prepare containers
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
			vectorNrmPos.push_back(vectorPos[i] + vectorNrm[i] * 0.05f);
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

		// get material name
		std::string materialName = "";
		if (shape.mesh.material_ids[0] >= 0)
			materialName = materials[shape.mesh.material_ids[0]].name;

		// get image from material
		VulkanMaterial* material = getOrDefault_Material(materialName);

		// create mesh
		VulkanMesh* mesh = new VulkanMesh_obj(
			renderer->device,
			renderer->pipeline_obj,
			material,
			vectorPos,
			vectorTex,
			vectorNrm);
		meshes.push_back(mesh);
		
		// create debug mesh
		VulkanMesh* meshDebug = new VulkanMesh_lines(
				renderer->device,
				renderer->pipeline_line,
				vectorNrmPos,
				vectorNrmCol);
		meshes.push_back(meshDebug);

		// store local meshes
		local_meshes.push_back(mesh);
		local_meshes_debug.push_back(meshDebug);
	}

	// create model asset and add to map
	VulkanModelAsset* modelAsset = new VulkanModelAsset();
	modelAsset->meshes.insert(modelAsset->meshes.end(), local_meshes.begin(), local_meshes.end());
	modelAsset->meshesDebug.insert(modelAsset->meshesDebug.end(), local_meshes_debug.begin(), local_meshes_debug.end());
	add_ModelAsset(modelAsset, fileName); 
	return true;
}
