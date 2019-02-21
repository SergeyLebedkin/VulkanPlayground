#pragma once
#include <glm/mat4x4.hpp>
#include "vulkan_pipelines.hpp"

// VulkanMesh
class VulkanMesh
{
protected:
	// material
	VulkanDevice&       device;
	VulkanShader&       shader;
	VulkanDescriptorSet descriptorSet{};
	VulkanBuffer        bufferMVP{};
	int32_t             vertexCount{};
public:
	VulkanMesh(VulkanDevice& device, VulkanShader& shader);
	virtual ~VulkanMesh();

	// set image
	void setImage(
		VulkanImage&   image,
		VulkanSampler& sampler,
		uint32_t       binding);

	// draw
	virtual void draw(
		VulkanPipeline&      pipeline,
		VulkanCommandBuffer& commandBuffer,
		glm::mat4&           matProj,
		glm::mat4&           matView,
		glm::mat4&           matModl) = 0;
};

// VulkanMesh
class VulkanMesh_obj : public VulkanMesh
{
protected:
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferTex{};
	VulkanBuffer bufferNrm{};
public:
	// constructor
	VulkanMesh_obj(
		VulkanDevice&           device,
		VulkanShader&           shader,
		std::vector<glm::vec3>& pos,
		std::vector<glm::vec2>& tex,
		std::vector<glm::vec3>& nrm);
	~VulkanMesh_obj();
	void draw(
		VulkanPipeline&      pipeline,
		VulkanCommandBuffer& commandBuffer,
		glm::mat4&           matProj,
		glm::mat4&           matView,
		glm::mat4&           matModl) override;
};

// VulkanMesh_lines
struct VulkanMesh_lines : public VulkanMesh
{
protected:
	// buffers
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferCol{};
public:
	// constructor
	VulkanMesh_lines(
		VulkanDevice&           device,
		VulkanShader&           shader,
		std::vector<glm::vec3>& pos,
		std::vector<glm::vec4>& col);
	~VulkanMesh_lines();
	void draw(
		VulkanPipeline&      pipeline,
		VulkanCommandBuffer& commandBuffer,
		glm::mat4&           matProj,
		glm::mat4&           matView,
		glm::mat4&           matModl) override;
};
