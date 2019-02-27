#pragma once
#include "vulkan_material.hpp"
#include <glm/mat4x4.hpp>
#include <array>

// VulkanMesh
class VulkanMesh
{
protected:
	VulkanDevice&   device;
	VulkanPipeline& pipeline;
public:
	VulkanMesh(
		VulkanDevice&   device,
		VulkanPipeline& pipeline
	) : device(device), pipeline(pipeline) {};
	virtual ~VulkanMesh() {};
	virtual void draw(VulkanCommandBuffer& commandBuffer);
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
		VulkanPipeline& pipeline,
		VulkanMaterial* material) :
		VulkanMesh(device, pipeline), material(material) {};
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
	uint32_t     vertexCount;
public:
	// constructor
	VulkanMesh_gui(
		VulkanDevice&                       device,
		VulkanPipeline&                     pipeline,
		VulkanMaterial*                     material,
		std::vector<VertexStruct_P4_C4_T2>& verts);
	~VulkanMesh_gui();
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
	// constructor
	VulkanMesh_indexed(
		VulkanDevice&          device,
		VulkanPipeline&        pipeline,
		VulkanMaterial*        material,
		std::vector<uint32_t>& indexes);
	~VulkanMesh_indexed();
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
	// constructor
	VulkanMesh_indexed_obj(
		VulkanDevice&           device,
		VulkanPipeline&         pipeline,
		VulkanMaterial*         material,
		std::vector<uint32_t>&  indexes,
		std::vector<glm::vec3>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm);
	~VulkanMesh_indexed_obj();
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
	// constructor
	VulkanMesh_obj(
		VulkanDevice&           device,
		VulkanPipeline&         pipeline,
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
	std::array<VkBuffer, 2>     bufferHandles;
	std::array<VkDeviceSize, 2> bufferOffsets;
protected:
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferCol{};
	uint32_t     vertexCount;
public:
	// constructor
	VulkanMesh_lines(
		VulkanDevice&           device,
		VulkanPipeline&         pipeline,
		std::vector<glm::vec3>& pos,
		std::vector<glm::vec4>& col);
	~VulkanMesh_lines();
	void draw(VulkanCommandBuffer& commandBuffer) override;
};
