#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

// calcTangentSpace
void calcTangentSpace(
	const std::vector<glm::vec4>& pos,
	const std::vector<glm::vec2>& tex,
	const std::vector<glm::vec3>& nrm,
	std::vector<glm::vec3>& tng,
	std::vector<glm::vec3>& bnm
);
