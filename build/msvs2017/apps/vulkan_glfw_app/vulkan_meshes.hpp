#pragma once
#include <glm/mat4x4.hpp>
#include "vulkan_pipelines.hpp"

// VulkanMaterial
class VulkanMaterial {};

// VulkanMesh
class VulkanMesh
{
protected:
	VulkanDevice&         device;
public:
	VulkanMesh(VulkanDevice& device) : device(device) {};
	virtual ~VulkanMesh() {};
	virtual void draw(VulkanCommandBuffer& commandBuffer) = 0;
};

// VulkanMesh_material
class VulkanMesh_material : public VulkanMesh
{
protected:
	// material
	VulkanMaterial* material{};
public:
	VulkanMesh_material(
		VulkanDevice&   device,
		VulkanMaterial* material) :
		VulkanMesh(device), material(material) {};
};

// VulkanMesh_gui
struct VulkanMesh_gui : public VulkanMesh_material
{
protected:
	// buffers
	VulkanBuffer bufferVerteces{};
	uint32_t     vertexCount;
public:
	// constructor
	VulkanMesh_gui(
		VulkanDevice&                       device,
		VulkanMaterial*                     material,
		std::vector<VertexStruct_P4_C4_T2>& verts);
	~VulkanMesh_gui();
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMesh_obj
class VulkanMesh_obj : public VulkanMesh_material
{
protected:
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferTex{};
	VulkanBuffer bufferNrm{};
	uint32_t     vertexCount;
public:
	// constructor
	VulkanMesh_obj(
		VulkanDevice&           device,
		VulkanMaterial*         material,
		std::vector<glm::vec3>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm);
	~VulkanMesh_obj();
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMesh_lines
struct VulkanMesh_lines : public VulkanMesh
{
protected:
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferCol{};
	uint32_t     vertexCount;
public:
	// constructor
	VulkanMesh_lines(
		VulkanDevice&           device,
		std::vector<glm::vec3>& pos,
		std::vector<glm::vec4>& col);
	~VulkanMesh_lines();
	void draw(VulkanCommandBuffer& commandBuffer) override;
};
