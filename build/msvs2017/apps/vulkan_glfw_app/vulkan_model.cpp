#include "vulkan_model.hpp"

// VulkanModel::draw
void VulkanModel::draw(
	VulkanPipeline&      pipeline,
	VulkanCommandBuffer& commandBuffer,
	glm::mat4&           matProj,
	glm::mat4&           matView)
{
	if (visible)
	{
		// draw meshes
		for (auto& mesh : meshes)
			mesh->draw(pipeline, commandBuffer, matProj, matView, matModel);

		// draw debug meshes
		if (visibleDebug)
			for (auto& mesh : meshesDebug)
				mesh->draw(pipeline, commandBuffer, matProj, matView, matModel);
	}
}
