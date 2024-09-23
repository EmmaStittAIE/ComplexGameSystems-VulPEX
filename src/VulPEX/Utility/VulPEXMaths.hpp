#pragma once

#include "glm/glm.hpp"

enum VulkanFormat
{
	eVec2 = (int)vk::Format::eR32G32Sfloat,
	eVec3 = (int)vk::Format::eR32G32B32Sfloat
};

// Int vectors
typedef glm::ivec2 IVec2;

// Float vectors
typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;

// Matrices
typedef glm::mat4 Mat4;

const float PI = 3.14159f;