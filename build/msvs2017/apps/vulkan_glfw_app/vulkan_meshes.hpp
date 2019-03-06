#pragma once
#include "vulkan_material.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <array>

// VulkanMesh
class VulkanMesh {
protected:
	// context
	VulkanContext& context;
public:
	// constructor and destructor
	VulkanMesh(VulkanContext& context) : context(context) {};
	virtual ~VulkanMesh() {};

	// draw
	virtual void draw(VulkanCommandBuffer& commandBuffer) = 0;
};

// VulkanMesh_material
class VulkanMesh_material : public VulkanMesh {
protected:
	// material
	VulkanMaterial* material{};
public:
	// constructor and destructor
	VulkanMesh_material(
		VulkanContext&  context,
		VulkanMaterial* material) :
		VulkanMesh(context), material(material) {};

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMesh_gui
struct VulkanMesh_gui : public VulkanMesh_material
{
protected:
	std::array<VkBuffer, 1>     bufferHandles;
	std::array<VkDeviceSize, 1> bufferOffsets;
protected:
	// buffers
	VulkanBuffer bufferVerteces{};
	uint32_t     vertexCount{};
public:
	VkPrimitiveTopology primitiveTopology{};
public:
	// constructor and destructor
	VulkanMesh_gui(
		VulkanContext&                      context,
		VulkanMaterial*                     material,
		std::vector<VertexStruct_P4_C4_T2>& verts,
		VkPrimitiveTopology                 primitiveTopology);
	~VulkanMesh_gui();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMesh_indexed
struct VulkanMesh_indexed : public VulkanMesh_material
{
protected:
	// buffers
	VulkanBuffer bufferIndexes{};
	uint32_t     indexCount;
public:
	// constructor and destructor
	VulkanMesh_indexed(
		VulkanContext&         context,
		VulkanMaterial*        material,
		std::vector<uint32_t>& indexes);
	~VulkanMesh_indexed();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMesh_indexed_obj
struct VulkanMesh_indexed_obj : public VulkanMesh_indexed
{
protected:
	std::array<VkBuffer, 3>     bufferHandles;
	std::array<VkDeviceSize, 3> bufferOffsets;
protected:
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferTex{};
	VulkanBuffer bufferNrm{};
	uint32_t     vertexCount;
public:
	// primitive topology
	VkPrimitiveTopology primitiveTopology{};
public:
	// constructor and destructor
	VulkanMesh_indexed_obj(
		VulkanContext&          context,
		VulkanMaterial*         material,
		std::vector<uint32_t>&  indexes,
		std::vector<glm::vec3>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm,
		VkPrimitiveTopology     primitiveTopology);
	~VulkanMesh_indexed_obj();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMesh_obj
class VulkanMesh_obj : public VulkanMesh_material
{
protected:
	std::array<VkBuffer, 3>     bufferHandles;
	std::array<VkDeviceSize, 3> bufferOffsets;
protected:
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferTex{};
	VulkanBuffer bufferNrm{};
	uint32_t     vertexCount;
public:
	// primitive topology
	VkPrimitiveTopology primitiveTopology{};
public:
	// constructor and destructor
	VulkanMesh_obj(
		VulkanContext&          context,
		VulkanMaterial*         material,
		std::vector<glm::vec3>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm,
		VkPrimitiveTopology     primitiveTopology);
	~VulkanMesh_obj();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMesh_debug
struct VulkanMesh_debug : public VulkanMesh
{
protected:
	std::array<VkBuffer, 2>     bufferHandles;
	std::array<VkDeviceSize, 2> bufferOffsets;
protected:
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferCol{};
	uint32_t     vertexCount;
public:
	// primitive topology
	VkPrimitiveTopology primitiveTopology{};
public:
	// constructor and destructor
	VulkanMesh_debug(
		VulkanContext&          context,
		std::vector<glm::vec3>& pos,
		std::vector<glm::vec4>& col,
		VkPrimitiveTopology     primitiveTopology);
	~VulkanMesh_debug();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};
