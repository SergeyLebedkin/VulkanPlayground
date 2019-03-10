#pragma once
#include "vulkan_material.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <array>

// VulkanMesh
class VulkanMesh : public VulkanContextObject {
public:
	// primitive topology
	VkPrimitiveTopology primitiveTopology{};
public:
	// constructor and destructor
	VulkanMesh(VulkanContext& context, VkPrimitiveTopology primitiveTopology) :
		VulkanContextObject(context), primitiveTopology(primitiveTopology) {};
	~VulkanMesh() {}

	// draw
	virtual void draw(VulkanCommandBuffer& commandBuffer) = 0;
};

// VulkanMesh_color
class VulkanMesh_color : public VulkanMesh {
protected:
	std::array<VkBuffer, 2>     bufferHandles;
	std::array<VkDeviceSize, 2> bufferOffsets;
protected:
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferCol{};
	uint32_t     vertexCount;
public:
	// constructor and destructor
	VulkanMesh_color(
		VulkanContext&          context,
		VkPrimitiveTopology     primitiveTopology,
		std::vector<glm::vec4>& pos,
		std::vector<glm::vec4>& col);
	~VulkanMesh_color();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMesh_material
class VulkanMesh_material : public VulkanMesh {
protected:
	// material
	VulkanMaterial* material{};
public:
	// constructor and destructor
	VulkanMesh_material(VulkanContext& context, VkPrimitiveTopology primitiveTopology) :
		VulkanMesh(context, primitiveTopology) {};
	~VulkanMesh_material() {}

	// set material
	void setMaterial(VulkanMaterial* material) { this->material = material; }
	VulkanMaterial* getMaterial() { return this->material; }

	// update
	void update(VulkanCommandBuffer& commandBuffer) override;

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
	// constructor and destructor
	VulkanMesh_gui(VulkanContext& context, VkPrimitiveTopology primitiveTopology,
		std::vector<VertexStruct_P4_C4_T2>& verts);
	~VulkanMesh_gui();

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
	// constructor and destructor
	VulkanMesh_obj(
		VulkanContext&          context,
		VkPrimitiveTopology     primitiveTopology,
		std::vector<glm::vec4>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm);
	~VulkanMesh_obj();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};
