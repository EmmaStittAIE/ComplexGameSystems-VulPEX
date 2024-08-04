#pragma once

#include <vector>
#include <string>

#include <vulkan/vulkan.hpp>

struct ShaderInfo
{
	std::vector<char> vertBytecode;
	std::vector<char> fragBytecode;
};

class GraphicsPipelineWrapper
{
	vk::RenderPass m_renderPass = nullptr;
	vk::PipelineLayout m_pipelineLayout = nullptr;
	vk::Pipeline m_graphicsPipeline = nullptr;

	vk::ShaderModule CreateShaderModule(vk::Device device, std::vector<char> bytecode);

	void CreateRenderPass(vk::Device device, vk::Format imageFormat);

public:
	void CreateGraphicsPipeline(vk::Device device, ShaderInfo shaderInfo, vk::Extent2D scExtent, vk::Format imageFormat);

	// Getters
	vk::Pipeline GetPipeline() const { return m_graphicsPipeline; };
	vk::RenderPass GetRenderPass() const { return m_renderPass; };

	// Cleanup
	void DestroyPipeline(vk::Device device);
};