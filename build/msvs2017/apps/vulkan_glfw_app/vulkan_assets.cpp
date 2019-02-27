#include "vulkan_assets.hpp"
#include "vulkan_loaders.hpp"

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
	createImageProcedural(renderer->device, 256, 256, imageDefault);

	// create dafult material
	materialDefault = new VulkanMaterial(
		renderer->device,
		renderer->pipelineLayout,
		renderer->descriptorSetLayout_material);
	materialDefault->setImage(imageDefault, renderer->samplerDefault, 0);

	// create default mesh
	meshDefault = new VulkanMesh_gui(
		renderer->device,
		renderer->pipeline_gui,
		materialDefault,
		vertices
	);

	// create default model
	modelAssetDefault = new VulkanModelAsset();
	modelAssetDefault->meshes.push_back(meshDefault);
};

// VulkanAssetManager::~VulkanAssetManager
VulkanAssetManager::~VulkanAssetManager()
{
	// destroy default handles
	delete modelAssetDefault;
	delete meshDefault;
	delete materialDefault;
	vulkanImageDestroy(renderer->device, imageDefault);

	// destroy handles
	for (auto& modelAsset : mapModelAssets) delete modelAsset.second;
	for (auto& material : mapMaterials) delete material.second;
	for (auto& image : mapImages) vulkanImageDestroy(renderer->device, *image.second);

	// delete meshes
	for (auto& mesh : meshes) delete mesh;

	// clear maps
	mapModelAssets.clear();
	mapMaterials.clear();
	mapImages.clear();
};

// VulkanAssetManager::add_Image
void VulkanAssetManager::add_Image(VulkanImage* image, const std::string name)
{
};

// VulkanAssetManager::add_Material
void VulkanAssetManager::add_Material(VulkanMaterial* material, const std::string name)
{
};

// VulkanAssetManager::add_ModelAsset
void VulkanAssetManager::add_ModelAsset(VulkanModelAsset* modelAsset, const std::string name)
{
};

// VulkanAssetManager::getOrDefault_Image
VulkanImage* VulkanAssetManager::getOrDefault_Image(const std::string name) 
{ 
	return nullptr;
};

// VulkanAssetManager::getOrDefault_Material
VulkanMaterial* VulkanAssetManager::getOrDefault_Material(const std::string name)
{ 
	return nullptr;
};

// VulkanAssetManager::getOrDefault_ModelAsset
VulkanModelAsset* VulkanAssetManager::getOrDefault_ModelAsset(const std::string name)
{ 
	return nullptr;
};

// VulkanAssetManager::createOrDefaut_Model
VulkanModel* VulkanAssetManager::createOrDefaut_Model(std::string name) 
{ 
	return nullptr;
};
