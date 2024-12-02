#pragma once

#include <cstddef>
#include <array>

#include <vulkan/vulkan.hpp>

#include "../../Utility/VulPEXMaths.hpp"

namespace DataStructures
{
	struct Vertex{
		Vec2 pos;
		Vec3 colour;
	
		Vertex(Vec2 pos_, Vec3 colour_ = {1, 1, 1}) { pos = pos_, colour = colour_; };

		static uint32_t GetSizeOf()
		{
			return sizeof(Vertex);
		}

		// Change size of array if number of variables changes
		static std::array<std::pair<vk::Format, uint32_t>, 2> GetVarInfo()
		{
			std::pair<vk::Format, size_t> vertPosInfo = {(vk::Format)VulkanFormat::eVec2, offsetof(Vertex, pos)};
			std::pair<vk::Format, size_t> vertNormInfo = {(vk::Format)VulkanFormat::eVec3, offsetof(Vertex, colour)};
			return {vertPosInfo, vertNormInfo};
		}
	};
}