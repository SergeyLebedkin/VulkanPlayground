#include "vulkan_assets.hpp"
#include "vulkan_loaders.hpp"
#include "vulkan_renderer.hpp"
#include <tiny_obj_loader.h>
#include <algorithm>

// VulkanAssetManager::VulkanAssetManager
VulkanAssetManager::VulkanAssetManager(VulkanContext& context)
	: context(context)
{
	// create default material
	defaultMaterial = new VulkanMaterial(context);
	defaultMaterial->setDiffuseImage(&context.defaultImage, &context.defaultSampler);
};

// VulkanAssetManager::~VulkanAssetManager
VulkanAssetManager::~VulkanAssetManager()
{
	// destroy mesh groups
	for (auto mesh_group : meshGroups)
		delete mesh_group;
	meshGroups.clear();
	// destroy meshes
	for (auto mesh_item : meshItems) {
		delete mesh_item->mesh;
		delete mesh_item->meshDebug;
	};
	meshItems.clear();
	// destroy materials
	for (auto material_item : materialItems)
		delete material_item->material;
	materialItems.clear();
	// destroy images
	for (auto image_item : imageItems)
		vulkanImageDestroy(context.device, *image_item->image);
	imageItems.clear();
	// clear default material
	delete defaultMaterial;
};

// VulkanAssetManager::getImageItemByName
VulkanImageItem* VulkanAssetManager::getImageItemByName(const std::string name) {
	for (auto image_item : imageItems) 
		if (image_item->name == name) 
			return image_item;
	return nullptr;
}

// VulkanAssetManager::getMaterialItemByName
VulkanMaterialItem* VulkanAssetManager::getMaterialItemByName(const std::string name) {
	for (auto material_item : materialItems) 
		if (material_item->name == name) 
			return material_item;
	return nullptr;
}

// VulkanAssetManager::getMeshItemByName
VulkanMeshItem* VulkanAssetManager::getMeshItemByName(const std::string name) {
	for (auto mesh_item : meshItems) 
		if (mesh_item->name == name) 
			return mesh_item;
	return nullptr;
}

// VulkanAssetManager::isImageExist
bool VulkanAssetManager::isImageExist(const std::string name) {
	for (auto image_item : imageItems) 
		if (image_item->name == name) 
			return true;
	return false;
}

// VulkanAssetManager::isExist_Material
bool VulkanAssetManager::isMaterialExist(const std::string name) {
	for (auto material_item : materialItems) 
		if (material_item->name == name) 
			return true;
	return false;
}

// VulkanAssetManager::isExist_ModelAsset
bool VulkanAssetManager::isMeshExist(const std::string name) {
	for (auto mesh_item : meshItems) 
		if (mesh_item->name == name) 
			return true;
	return false;
}

// VulkanAssetManager::isMeshGroupExist
bool VulkanAssetManager::isMeshGroupExist(const std::string name) {
	for (auto mesh_group : meshGroups) 
		if (mesh_group->name == name) 
			return true;
	return false;
}

// VulkanAssetManager::addImage
void VulkanAssetManager::addImage(const std::string name, VulkanImage* image) {
	// check name
	assert(name.size() > 0);
	// add if not exist
	if (!isImageExist(name))
		imageItems.push_back(new VulkanImageItem(name, image));
}

// VulkanAssetManager::addImageFromfile
void VulkanAssetManager::addImageFromfile(const std::string fileName) {
	// check name
	assert(fileName.size() > 0);
	// add if not exist
	if (!isImageExist(fileName)) {
		VulkanImage* image = new VulkanImage;
		loadImageFromFile(context.device, *image, fileName);
		imageItems.push_back(new VulkanImageItem(fileName, image));
	}
}

// VulkanAssetManager::addMaterial
void VulkanAssetManager::addMaterial(const std::string name, VulkanMaterial* material) {
	// check name
	assert(name.size() > 0);
	// add if not exist
	if (!isImageExist(name))
		materialItems.push_back(new VulkanMaterialItem(name, material));
}

// VulkanAssetManager::addMeterialFromObj
void VulkanAssetManager::addMeterialFromObj(
	const std::string          basePath,
	const tinyobj::material_t& material_obj)
{
	// check material name
	assert(material_obj.name.size() > 0);
	// create material
	VulkanMaterial* material = new VulkanMaterial(context);
	addMaterial(material_obj.name, material);
	VulkanImage* imageDeffuse = &context.defaultImage;
	// load diffuse image
	if (material_obj.diffuse_texname.size() > 0) {
		// load image from file
		std::string imageDiffuseFilePath = basePath + material_obj.diffuse_texname;
		addImageFromfile(imageDiffuseFilePath);
		// get loaded image
		VulkanImage* image = getImageByName(imageDiffuseFilePath);
		if (image) imageDeffuse = image;
	}
	// set diffuse image
	material->setDiffuseImage(imageDeffuse, &context.defaultSampler);
}

// VulkanAssetManager::addMesh
void VulkanAssetManager::addMesh(const std::string name, VulkanMeshMatObj* mesh) {
	// check name
	assert(name.size() > 0);
	// add if not exist
	if (!isImageExist(name))
		meshItems.push_back(new VulkanMeshItem(name, mesh, nullptr));
}

// VulkanAssetManager::addMeshGroup
void VulkanAssetManager::addMeshGroup(const std::string name, const std::vector<std::string> meshNames) {
	// check name
	assert(name.size() > 0);
	assert(meshNames.size() > 0);
	// check if mesh group exists
	if (isMeshGroupExist(name)) return;
	VulkanMeshGroup* mesh_group = new VulkanMeshGroup(name);
	for (auto& mesh_name : meshNames) {
		// find mesh
		VulkanMeshItem* mesh_item = getMeshItemByName(mesh_name);
		assert(mesh_item);
		// add mesh to mesh group
		mesh_group->meshes.push_back(mesh_item);
	}
	// add mesh group
	meshGroups.push_back(mesh_group);
}

// VulkanAssetManager::getImageByName
VulkanImage* VulkanAssetManager::getImageByName(const std::string name) {
	for (auto image_item : imageItems) 
		if (image_item->name == name) 
			return image_item->image;
	return false;
}

// VulkanAssetManager::getMaterialByName
VulkanMaterial* VulkanAssetManager::getMaterialByName(const std::string name) {
	for (auto material_item : materialItems) 
		if (material_item->name == name) 
			return material_item->material;
	return false;
}

// VulkanAssetManager::getMeshByName
VulkanMesh* VulkanAssetManager::getMeshByName(const std::string name) {
	for (auto mesh_item : meshItems) 
		if (mesh_item->name == name) 
			return mesh_item->mesh;
	return false;
}

// VulkanAssetManager::getMeshGroupByName
VulkanMeshGroup* VulkanAssetManager::getMeshGroupByName(const std::string name) {
	for (auto mesh_group : meshGroups) 
		if (mesh_group->name == name) 
			return mesh_group;
	return false;
}

// VulkanAssetManager::getImageNames
std::vector<std::string> VulkanAssetManager::getImageNames() {
	std::vector<std::string> names;
	for (auto image_item : imageItems)
		names.push_back(image_item->name);
	return names;
}

// VulkanAssetManager::getMaterialNames
std::vector<std::string> VulkanAssetManager::getMaterialNames() {
	std::vector<std::string> names;
	for (auto material_item : materialItems)
		names.push_back(material_item->name);
	return names;
}

// VulkanAssetManager::getMeshNames
std::vector<std::string> VulkanAssetManager::getMeshNames() {
	std::vector<std::string> names;
	for (auto mesh_item : meshItems)
		names.push_back(mesh_item->name);
	return names;
}

// VulkanAssetManager::getMeshGroupNames
std::vector<std::string> VulkanAssetManager::getMeshGroupNames() {
	std::vector<std::string> names;
	for (auto mesh_group : meshGroups)
		names.push_back(mesh_group->name);
	return names;
}

// VulkanAssetManager::createModelByMeshNames
VulkanModel* VulkanAssetManager::createModelByMeshNames(const std::vector<std::string> names) {
	// create model
	VulkanModel* model = new VulkanModel(context);
	// add meshes
	for (auto& name : names) {
		for (auto& mesh_item : meshItems) {
			if (mesh_item->name == name) {
				if (mesh_item->mesh) 
					model->meshes.push_back(mesh_item->mesh);
				if (mesh_item->meshDebug) 
					model->meshes_debug.push_back(mesh_item->meshDebug);
			}
		}
	}
	return model;
}

// VulkanAssetManager::createModelByMeshGroupName
VulkanModel* VulkanAssetManager::createModelByMeshGroupName(const std::string name) {
	// get mesh group by name
	VulkanMeshGroup* mesh_group = getMeshGroupByName(name);
	if (mesh_group) {
		// create model
		VulkanModel* model = new VulkanModel(context);
		// add meshes to model
		for (auto& mesh_item : mesh_group->meshes) {
			if (mesh_item->mesh) 
				model->meshes.push_back(mesh_item->mesh);
			if (mesh_item->meshDebug) 
				model->meshes_debug.push_back(mesh_item->meshDebug);
		}
		return model;
	}
	return nullptr;
}

// VulkanAssetManager::loadFromFileObj
std::vector<std::string> VulkanAssetManager::loadFromFileObj(
	const std::string fileName,
	const std::string basePath)
{
	// load and parse obj file
	std::string warm, err;
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	tinyobj::LoadObj(&attribs, &shapes, &materials, &warm, &err, fileName.data(), basePath.data(), true);
	assert(shapes.size() > 0);

	// load materials
	for (const auto& material_obj : materials)
		addMeterialFromObj(basePath, material_obj);

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

	// mesh names
	std::vector<std::string> mesh_names;

	// allocate buffers mesh
	std::vector<glm::vec4> vectorPos{};
	std::vector<glm::vec3> vectorNrm{};
	std::vector<glm::vec3> vectorTan{};
	std::vector<glm::vec3> vectorBit{};
	std::vector<glm::vec2> vectorTex{};
	// allocate buffers normals
	std::vector<glm::vec4> vectorNrmPos{};
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
				vectorPos.push_back(glm::vec4(
					(attribs.vertices[3 * index.vertex_index + 0] - centerPos.x) * scale,
					(attribs.vertices[3 * index.vertex_index + 1] - centerPos.y) * scale,
					(attribs.vertices[3 * index.vertex_index + 2] - centerPos.z) * scale, 1.0f));
			else vectorPos.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

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
			glm::vec4& v0 = vectorPos[i + 0];
			glm::vec4& v1 = vectorPos[i + 1];
			glm::vec4& v2 = vectorPos[i + 2];

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
			vectorNrmPos.push_back(vectorPos[i] + glm::vec4(vectorNrm[i] * 0.05f, 1.0f));
			vectorNrmCol.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			vectorNrmCol.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			vectorNrmPos.push_back(vectorPos[i]);
			vectorNrmPos.push_back(vectorPos[i] + glm::vec4(vectorTan[i] * 0.05f, 1.0f));
			vectorNrmCol.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			vectorNrmCol.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

			vectorNrmPos.push_back(vectorPos[i]);
			vectorNrmPos.push_back(vectorPos[i] + glm::vec4(vectorBit[i] * 0.05f, 1.0f));
			vectorNrmCol.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
			vectorNrmCol.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		}

		// get material name
		std::string materialName = "";
		if (shape.mesh.material_ids[0] >= 0)
			materialName = materials[shape.mesh.material_ids[0]].name;

		// get image from material
		VulkanMaterial* material = getMaterialByName(materialName);
		if (!material) material = defaultMaterial;

		// create mesh
		VulkanMeshMatObj* mesh = new VulkanMeshMatObj(context,
			vectorPos, vectorTex, vectorNrm);
		mesh->material = material;
		mesh->materialUsage = VULKAN_MATERIAL_USAGE_COLOR_TEXTURE;
		mesh->primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		//mesh->materialUsage = VULKAN_MATERIAL_USAGE_COLOR;

		// create debug mesh
		//VulkanMeshMatObj* mesh_debug = new VulkanMesh_color(
		//	context, VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
		//	vectorNrmPos, vectorNrmCol);

		// create and add mesh item
		VulkanMeshItem* mesh_item = new VulkanMeshItem(shape.name, mesh, nullptr);
		meshItems.push_back(mesh_item);

		// store local meshes
		mesh_names.push_back(shape.name);
	}
	addMeshGroup(fileName, mesh_names);
	return mesh_names;
}