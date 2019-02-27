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
	VulkanRender* renderer;
protected:
	uint64_t imageUnicIndex{};
	uint64_t materialUnicIndex{};
	uint64_t modelAssetUnicIndex{};

	VulkanImage       imageDefault{};
	VulkanMaterial*   materialDefault{};
	VulkanMesh*       meshDefault{};
	VulkanModelAsset* modelAssetDefault{};

	// asset maps
	std::vector<VulkanMesh*>                 meshes{};
	std::map<std::string, VulkanImage*>      mapImages{};
	std::map<std::string, VulkanMaterial*>   mapMaterials{};
	std::map<std::string, VulkanModelAsset*> mapModelAssets{};
public:
	// constructor and destructor
	VulkanAssetManager(VulkanRender* renderer);
	~VulkanAssetManager();

	// add functions
	void add_Image(VulkanImage* image, const std::string name = "");
	void add_Material(VulkanMaterial* material, const std::string name = "");
	void add_ModelAsset(VulkanModelAsset* modelAsset, const std::string name = "");

	// get or default functions
	VulkanImage*      getOrDefault_Image(const std::string name);
	VulkanMaterial*   getOrDefault_Material(const std::string name);
	VulkanModelAsset* getOrDefault_ModelAsset(const std::string name);

	// create functions
	VulkanModel* createOrDefaut_Model(std::string name);
};
