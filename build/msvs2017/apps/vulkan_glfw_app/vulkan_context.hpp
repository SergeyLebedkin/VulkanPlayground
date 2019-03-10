#pragma once
#include <vktoolkit.hpp>

// VulkanContext
class VulkanContext {
public:
	// instance and device
	VulkanInstance instance{};
	VulkanDevice   device{};
public:
	// descriptor set layouts
	VulkanDescriptorSetLayout descriptorSetLayout_material{};
	VulkanDescriptorSetLayout descriptorSetLayout_model{};
	VulkanDescriptorSetLayout descriptorSetLayout_scene{};
	// pipeline layout
	VulkanPipelineLayout pipelineLayout;
public:
	VulkanImage   defaultImage;
	VulkanSampler defaultSampler;
private:
	void createDefaultImage();
public:
	// constructor and destructor
	VulkanContext(
		std::vector<const char *>& enabledInstanceLayerNames,
		std::vector<const char *>& enabledInstanceExtensionNames,
		std::vector<const char *>& enabledDeviceExtensionNames,
		VkPhysicalDeviceFeatures&  physicalDeviceFeatures);
	~VulkanContext();
};

// VulkanContextObject
class VulkanContextObject {
protected:
	// parent context
	VulkanContext & context;
public:
	// constructor and destructor
	VulkanContextObject(VulkanContext& context) : context(context) {}
	virtual ~VulkanContextObject() {}

	// update
	virtual void update(VulkanCommandBuffer& commandBuffer) {};
};

/*
// VesuvioEngine
namespace VesuvioEngine
{
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
	public:
		// constructor and destructor
		VulkanMesh_color(VulkanContext& context, VkPrimitiveTopology primitiveTopology) :
			VulkanMesh(context, primitiveTopology) {};
		~VulkanMesh_color() {}

		// draw
		void draw(VulkanCommandBuffer& commandBuffer) override {
			// empty :(
		};
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
		void update(VulkanCommandBuffer& commandBuffer) override {
			// update material if exists
			if (material)
				material->update(commandBuffer);
		};

		// draw
		void draw(VulkanCommandBuffer& commandBuffer) override {
			if (material)
				material->bind(commandBuffer);
		};
	};

	// VulkanModel
	class VulkanModel : public VulkanContextObject {
	protected:
		// model matrix buffer
		VulkanBuffer bufferModelMatrix{};
	public:
		// model matrix
		float modelMatrix[16];
	public:
		// meshes
		std::vector<VulkanMesh_material*> meshes{};
		std::vector<VulkanMesh_color*> meshes_debug{};
	public:
		// constructor and destructor
		VulkanModel(VulkanContext& context) :
			VulkanContextObject(context) {};
		~VulkanModel() {}

		// update
		void update(VulkanCommandBuffer& commandBuffer) override {
			// update model matrix
			vkCmdUpdateBuffer(commandBuffer.commandBuffer, bufferModelMatrix.buffer, 0, sizeof(float)*16, modelMatrix);
			// update meshes
			for (auto& mesh : meshes)
				mesh->update(commandBuffer);
			// update debug meshes
			for (auto& mesh : meshes_debug)
				mesh->update(commandBuffer);
		};

		// bind
		virtual void bind(VulkanCommandBuffer& commandBuffer) {
			// bind descriptor set
			vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				context.pipelineLayout.pipelineLayout, 1, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
		};
	};

	// VulkanScene
	class VulkanScene : public VulkanContextObject {
	protected:
		// scene descriptor set
		VulkanDescriptorSet descriptorSet{};
		// scene matrices buffer
		VulkanBuffer sceneMatrices{};
	public:
		// view-projection matrices
		float projectionMatrix[16];
		float viewMatrix[16];
	public:
		// models
		std::vector<VulkanModel*> models{};
	public:
		// constructor and destructor
		VulkanScene(VulkanContext& context) :
			VulkanContextObject(context) {};
		~VulkanScene() {}

		// update
		void update(VulkanCommandBuffer& commandBuffer) override {
			// update model matrix
			vkCmdUpdateBuffer(commandBuffer.commandBuffer, sceneMatrices.buffer, sizeof(float) * 16 * 0, sizeof(float) * 16, projectionMatrix);
			vkCmdUpdateBuffer(commandBuffer.commandBuffer, sceneMatrices.buffer, sizeof(float) * 16 * 1, sizeof(float) * 16, viewMatrix);
			// update models
			for (auto& model : models)
				model->update(commandBuffer);
		};

		// bind
		virtual void bind(VulkanCommandBuffer& commandBuffer) {
			// bind descriptor set
			vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				context.pipelineLayout.pipelineLayout, 1, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
		};
	};
}
*/