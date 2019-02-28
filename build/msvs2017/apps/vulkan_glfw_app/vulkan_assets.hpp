#pragma once
#include "vulkan_model.hpp"
#include <map>

// VulkanRender
class VulkanRender;

// VulkanModelAsset 
struct VulkanModelAsset {
	std::vector<VulkanMesh*> meshes{};
	std::vector<VulkanMesh*> meshesDebug{};
};

// VulkanAssetManager
class VulkanAssetManager {
protected:
	VulkanRender* renderer{};
protected:
	// default assets
	VulkanImage       defaultImage{};
	VulkanMaterial*   defaultMaterial{};
	VulkanMesh*       defaultMesh{};
	VulkanModelAsset* defaultModelAsset{};

	// meshes
	std::vector<VulkanMesh*> meshes{};

	// assets maps
	std::map<std::string, VulkanImage*>      mapImages{};
	std::map<std::string, VulkanMaterial*>   mapMaterials{};
	std::map<std::string, VulkanModelAsset*> mapModelAssets{};
public:
	// constructor and destructor
	VulkanAssetManager(VulkanRender* renderer);
	~VulkanAssetManager();

	// is exists functions
	bool isExist_Image(const std::string name);
	bool isExist_Material(const std::string name);
	bool isExist_ModelAsset(const std::string name);

	// add functions
	void add_Image(VulkanImage* image, const std::string name = "");
	void add_Material(VulkanMaterial* material, const std::string name = "");
	void add_ModelAsset(VulkanModelAsset* modelAsset, const std::string name = "");

	// get or default functions
	VulkanImage*      getOrDefault_Image(const std::string name);
	VulkanMaterial*   getOrDefault_Material(const std::string name);
	VulkanModelAsset* getOrDefault_ModelAsset(const std::string name);

	// get or load from file image
	VulkanImage* getOrLoadFromFile_Image(const std::string fileName);

	// create functions
	VulkanModel* createOrDefaut_Model(std::string name);

	// load from obj file with materials
	bool loadFileObj(std::string fileName, std::string basePath);
};
