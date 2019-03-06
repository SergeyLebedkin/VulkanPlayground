#pragma once
#include "vulkan_renderer.hpp"
#include <tiny_obj_loader.h>
#include <memory>

// VulkanContext
class VulkanContext;

// VulkanImageItem
struct VulkanImageItem {
	std::string  name{};
	VulkanImage* image{};
	VulkanImageItem(std::string  name,VulkanImage* image) :
		name(name),
		image(image) {}
};

// VulkanMaterialItem
struct VulkanMaterialItem {
	std::string     name{};
	VulkanMaterial* material{};
	VulkanMaterialItem(std::string name, VulkanMaterial* material) :
		name(name),
		material(material) {}
};

// VulkanMeshItem
struct VulkanMeshItem {
	std::string       name{};
	VulkanMesh*       mesh{};
	VulkanMesh_debug* meshDebug{};
	VulkanMeshItem(
		std::string       name,
		VulkanMesh*       mesh,
		VulkanMesh_debug* meshDebug) :
		name(name),
		mesh(mesh),
		meshDebug(meshDebug) {}
};

// VulkanMeshGroup
struct VulkanMeshGroup {
	std::string name{};
	std::vector<VulkanMeshItem*> meshes{};
	std::vector<std::string>     childs{};
	VulkanMeshGroup(std::string name) :	name(name) {};
};

// VulkanAssetManager
class VulkanAssetManager {
protected:
	VulkanContext& context;
protected:
	// base items
	std::vector<VulkanImageItem*>    imageItems{};
	std::vector<VulkanMaterialItem*> materialItems{};
	std::vector<VulkanMeshItem*>     meshItems{};
	// mesh groups
	std::vector<VulkanMeshGroup*> meshGroups{};
protected:
	// get items functions
	VulkanImageItem*    getImageItemByName(const std::string name);
	VulkanMaterialItem* getMaterialItemByName(const std::string name);
	VulkanMeshItem*     getMeshItemByName(const std::string name);
public:
	// default material
	VulkanMaterial* defaultMaterial{};
public:
	// constructor and destructor
	VulkanAssetManager(VulkanContext& context);
	~VulkanAssetManager();

	// is exists functions
	bool isImageExist(const std::string name);
	bool isMaterialExist(const std::string name);
	bool isMeshExist(const std::string name);
	bool isMeshGroupExist(const std::string name);

	// add functions
	void addImage(const std::string name, VulkanImage* image);
	void addImageFromfile(const std::string fileName);
	void addMaterial(const std::string name, VulkanMaterial* material);
	void addMeterialFromObj(const std::string basePath, const tinyobj::material_t& material_obj);
	void addMesh(const std::string name, VulkanMesh* mesh);
	void addMeshGroup(const std::string name, const std::vector<std::string> meshNames);

	// get functions
	VulkanImage*     getImageByName(const std::string name);
	VulkanMaterial*  getMaterialByName(const std::string name);
	VulkanMesh*      getMeshByName(const std::string name);
	VulkanMeshGroup* getMeshGroupByName(const std::string name);

	// get names functions
	std::vector<std::string> getImageNames();
	std::vector<std::string> getMaterialNames();
	std::vector<std::string> getMeshNames();
	std::vector<std::string> getMeshGroupNames();

	// model functions
	VulkanModel* createModelByMeshNames(const std::vector<std::string> names);
	VulkanModel* createModelByMeshGroupName(const std::string name);

	// load obj file
	std::vector<std::string> loadFromFileObj(const std::string fileName, const std::string basePath);
};
