#include "vulkan_geometry.hpp"
#include "glm/mat4x4.hpp"

// calcTangentSpace
void calcTangentSpace(
	const std::vector<glm::vec4>& pos,
	const std::vector<glm::vec2>& tex,
	const std::vector<glm::vec3>& nrm,
	std::vector<glm::vec3>& tng,
	std::vector<glm::vec3>& bnm)
{
	// calculate bi-normal and tangent
	tng.clear();
	bnm.clear();
	tng.reserve(pos.size());
	bnm.reserve(pos.size());
	for (size_t i = 0; i < pos.size(); i += 3) {
		// Shortcuts for vertices
		glm::vec4 v0 = pos[i + 0];
		glm::vec4 v1 = pos[i + 1];
		glm::vec4 v2 = pos[i + 2];

		// Shortcuts for UVs
		glm::vec2 uv0 = tex[i + 0];
		glm::vec2 uv1 = tex[i + 1];
		glm::vec2 uv2 = tex[i + 2];

		// Edges of the triangle : position delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);
		glm::vec3 binormal = glm::normalize((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r);

		tng.push_back(tangent);
		tng.push_back(tangent);
		tng.push_back(tangent);
		bnm.push_back(binormal);
		bnm.push_back(binormal);
		bnm.push_back(binormal);
	}
}
