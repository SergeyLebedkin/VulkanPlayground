#pragma once
#include "vulkan_material.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <array>

//	VulkanMesh
//		VulkanMeshMaterial
//			VulkanMeshMatObj
//				VulkanMeshMatObjIndexed
//				VulkanMeshMatObjTBN
//					VulkanMeshMatObjTBNIndexed
//			VulkanMeshMatObjSkinned
//				VulkanMeshMatObjSkinnedIndexed
//				VulkanMeshMatObjSkinnedTBN
//					VulkanMeshMatObjSkinnedTBNIndexed

// VulkanMesh
class VulkanMesh : public VulkanContextDrawableObject {
public:
	// primitive topology
	VkPrimitiveTopology primitiveTopology{};
public:
	// constructor and destructor
	VulkanMesh(VulkanContext& context) :
		VulkanContextDrawableObject(context) {};
	~VulkanMesh() {}
};

// VulkanMeshMaterial
class VulkanMeshMaterial : public VulkanMesh {
public:
	// handle to material (can be NULL)
	VulkanMaterial* material{};
	// material usage
	VulkanMaterialUsage materialUsage{};
public:
	// constructor and destructor
	VulkanMeshMaterial(VulkanContext& context) :
		VulkanMesh(context) {};
};

// VulkanMeshMatObj
class VulkanMeshMatObj : public VulkanMeshMaterial {
protected:
	// buffer handles
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
	VulkanMeshMatObj(
		VulkanContext&          context,
		std::vector<glm::vec4>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm);
	~VulkanMeshMatObj();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMeshMatObjIndexed
class VulkanMeshMatObjIndexed : public VulkanMeshMatObj {
protected:
	// index buffer
	VulkanBuffer bufferInd{};
	uint32_t     indexCount;
public:
	// constructor and destructor
	VulkanMeshMatObjIndexed(
		VulkanContext&          context,
		std::vector<glm::vec4>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm,
		std::vector<uint32_t>&  ind);
	~VulkanMeshMatObjIndexed();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMeshMatObjTBN
class VulkanMeshMatObjTBN : public VulkanMeshMatObj {
protected:
	// BPR buffer handles handles
	std::array<VkBuffer, 2>     bufferHandlesTB;
	std::array<VkDeviceSize, 2> bufferOffsetsTB;
protected:
	// buffers
	VulkanBuffer bufferTng{};
	VulkanBuffer bufferBnm{};
public:
	// constructor and destructor
	VulkanMeshMatObjTBN(
		VulkanContext&          context,
		std::vector<glm::vec4>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm,
		std::vector<glm::vec3>& tng,
		std::vector<glm::vec3>& bnm);
	~VulkanMeshMatObjTBN();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMeshMatObjTBNIndexed
class VulkanMeshMatObjTBNIndexed : public VulkanMeshMatObjTBN {
protected:
	// index buffer
	VulkanBuffer bufferInd{};
	uint32_t     indexCount;
public:
	// constructor and destructor
	VulkanMeshMatObjTBNIndexed(
		VulkanContext&          context,
		std::vector<glm::vec4>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm,
		std::vector<glm::vec3>& tng,
		std::vector<glm::vec3>& bnm,
		std::vector<uint32_t>&  ind);
	~VulkanMeshMatObjTBNIndexed();

	// draw
	void draw(VulkanCommandBuffer& commandBuffer) override;
};

// VulkanMeshMatObjSkinned
class VulkanMeshMatObjSkinned : public VulkanMeshMaterial {};
