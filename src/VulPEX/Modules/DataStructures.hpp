#pragma once

#include <array>
#include <cstddef>

#include <vulkan/vulkan.hpp>

#include "../Utility/VulPEXMaths.hpp"

// This template, and the resulting consequences to the Vertex struct, are all to ensure that, were someone to make a custom
// vertex struct deriving from VertexBase, they must implement certain static functions before the code will compile
template <typename VecDerived>
struct VertexBase {
private:
	static vk::VertexInputBindingDescription getInputBindingDescription()
	{
		return VecDerived::getInputBindingDescriptionInternal();
	};

	static std::array<vk::VertexInputAttributeDescription, 2> getInputAttributeDescriptions()
	{
		return VecDerived::getInputAttributeDescriptionsInternal();
	};
};

namespace DataStructures
{
	struct Vertex : public VertexBase<Vertex> {
		glm::vec2 pos;
		glm::vec3 colour;
	
		Vertex(glm::vec2 pos_, glm::vec3 colour_) { pos = pos_, colour = colour_; };

		friend VertexBase<Vertex>;
	protected:
		static vk::VertexInputBindingDescription getInputBindingDescriptionInternal()
		{
			vk::VertexInputBindingDescription inputBindingDescription(
				0,								//binding
				sizeof(Vertex),					//stride
				vk::VertexInputRate::eVertex	//inputRate
			);

			return inputBindingDescription;
		}

		static std::array<vk::VertexInputAttributeDescription, 2> getInputAttributeDescriptionsInternal()
		{
			vk::VertexInputAttributeDescription posAttrDesc(
				0,									//location
				0,									//binding
				(vk::Format)VulkanFormat::eVec3,	//format
				offsetof(Vertex, pos)				//offset
			);

			vk::VertexInputAttributeDescription colourAttrDesc(
				0,									//location
				0,									//binding
				(vk::Format)VulkanFormat::eVec2,	//format
				offsetof(Vertex, colour)			//offset
			);


			std::array<vk::VertexInputAttributeDescription, 2> inputAttributeDescriptions({posAttrDesc, colourAttrDesc});

			return inputAttributeDescriptions;
		}
	};
}