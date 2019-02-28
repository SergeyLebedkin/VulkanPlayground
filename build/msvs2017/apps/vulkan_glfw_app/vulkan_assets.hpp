#pragma once
#include "vulkan_model.hpp"
#include <tiny_obj_loader.h>
#include <memory>

// VulkanRender
class VulkanRender;

// VulkanImageItem
struct VulkanImageItem {
	std::string  name{};
	VulkanImage* image{};
	VulkanImageItem(std::string name, VulkanImage* image) : 
		name(name), image(image) {}
};

// VulkanMaterialItem
struct VulkanMaterialItem {
	std::string     name{};
	VulkanMaterial* material{};
	VulkanMaterialItem(std::string name, VulkanMaterial* material) : 
		name(name), material(material) {}
};

// VulkanMeshItem
struct VulkanMeshItem {
	std::string name{};
	VulkanMesh* mesh{};
	VulkanMesh* meshDebug{};
	VulkanMeshItem(std::string name, VulkanMesh* mesh, VulkanMesh* meshDebug) :
		name(name), mesh(mesh), meshDebug(meshDebug) {}
};

// VulkanAssetManager
class VulkanAssetManager {
protected:
	VulkanRender* renderer{};
protected:
	// base items
	std::vector<VulkanImageItem*>    imageItems{};
	std::vector<VulkanMaterialItem*> materialItems{};
	std::vector<VulkanMeshItem*>     meshItems{};
public:
	// constructor and destructor
	VulkanAssetManager(VulkanRender* renderer);
	~VulkanAssetManager();

	// is exists functions
	bool isImageExist(const std::string name);
	bool isMaterialExist(const std::string name);
	bool isMeshExist(const std::string name);

	// add functions
	void addImage(const std::string name, VulkanImage* image);
	void addImageFromfile(const std::string fileName);
	void addMaterial(const std::string name, VulkanMaterial* material);
	void addMesh(const std::string name, VulkanMesh* mesh);

	// get functions
	VulkanImage*    getImageByName(const std::string name);
	VulkanMaterial* getMaterialByName(const std::string name);
	VulkanMesh*     getMeshByName(const std::string name);

	// get names functions
	std::vector<std::string> getImageNames();
	std::vector<std::string> getMaterialNames();
	std::vector<std::string> getMeshNames();

	// model functions
	VulkanModel* createModelByMeshNames(const std::vector<std::string> names);

	// load obj mesh and material utils
	void addMeterialFromObj(const std::string basePath, const tinyobj::material_t& material_obj);

	// load obj file
	std::vector<std::string> loadFromFileObj(const std::string fileName, const std::string basePath);
};
