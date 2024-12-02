#include "GraphicsPipelineWrapper.hpp"

// Private
vk::ShaderModule GraphicsPipelineWrapper::CreateShaderModule(vk::Device device, std::vector<char> bytecode)
{
	vk::ShaderModuleCreateInfo moduleInfo(
		{},							//flags
		bytecode.size(),			//codeSize
		(uint32_t*)bytecode.data(),	//pCode | This expects a uint32_t
		nullptr						//pNext
	);

	return device.createShaderModule(moduleInfo);
}

void GraphicsPipelineWrapper::CreateRenderPass(vk::Device device, vk::Format imageFormat)
{
	// This is where we'd set up multisampling, and a few other things
	vk::AttachmentDescription colourAttachmentDesc(
		{},									//flags
		imageFormat,						//format
		vk::SampleCountFlagBits::e1,		//samples
		vk::AttachmentLoadOp::eClear,		//loadOp
		vk::AttachmentStoreOp::eStore,		//storeOp
		vk::AttachmentLoadOp::eDontCare,	//stencilLoadOp
		vk::AttachmentStoreOp::eDontCare,	//stencilStoreOp
		vk::ImageLayout::eUndefined,		//initialLayout
		vk::ImageLayout::ePresentSrcKHR		//finalLayout
	);

	vk::AttachmentReference colourAttachmentRef(
		0,											//attachment
		vk::ImageLayout::eColorAttachmentOptimal	//layout
	);

	vk::SubpassDescription subpassDesc(
		{},									//flags
		vk::PipelineBindPoint::eGraphics	//pipelineBindPoint
	);
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.pColorAttachments = &colourAttachmentRef;

	vk::SubpassDependency dependency(
		vk::SubpassExternal,								//srcSubpass
		0,													//dstSubpass
		vk::PipelineStageFlagBits::eColorAttachmentOutput,	//srcStageMask
		vk::PipelineStageFlagBits::eColorAttachmentOutput,	//dstStageMask
		vk::AccessFlagBits::eNone,							//srcAccessMask
		vk::AccessFlagBits::eColorAttachmentWrite			//dstAccessMask
	);

	vk::RenderPassCreateInfo renderPassInfo(
		{},							//flags
		1,							//attachmentCount
		&colourAttachmentDesc,		//pAttachments
		1,							//subpassCount
		&subpassDesc,				//pSubpasses
		1,							//dependencyCount
		&dependency					//pDependencies
	);

	m_renderPass = device.createRenderPass(renderPassInfo);
}

// Public
void GraphicsPipelineWrapper::CreateGraphicsPipeline(vk::Device device, ShaderInfo shaderInfo, vk::Extent2D scExtent, vk::Format imageFormat, uint32_t sizeOfVertex,
													 std::pair<vk::Format, uint32_t>* vertexVarsInfo, size_t vertexVarsInfoCount)
{
	CreateRenderPass(device, imageFormat);

	vk::ShaderModule vertShaderModule = CreateShaderModule(device, shaderInfo.vertBytecode);
	vk::ShaderModule fragShaderModule = CreateShaderModule(device, shaderInfo.fragBytecode);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo(
		{},									//flags
		vk::ShaderStageFlagBits::eVertex,	//stage
		vertShaderModule,					//module
		"main"								//pName
	);

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo(
		{},									//flags
		vk::ShaderStageFlagBits::eFragment,	//stage
		fragShaderModule,					//module
		"main"								//pName
	);

	vk::PipelineShaderStageCreateInfo shaderStageInfos[] = { vertShaderStageInfo, fragShaderStageInfo };

	vk::VertexInputBindingDescription inputBindingDescription(
		0,								//binding
		sizeOfVertex,					//stride
		vk::VertexInputRate::eVertex	//inputRate
	);

	std::vector<vk::VertexInputAttributeDescription> inputAttributeDescriptions;
	for (int i = 0; i < vertexVarsInfoCount; i++)
	{
		vk::VertexInputAttributeDescription vertInputAttribDesc(
			i,							//location
			0,							//binding
			vertexVarsInfo[i].first,	//format
			vertexVarsInfo[i].second	//offset
		);

		inputAttributeDescriptions.push_back(vertInputAttribDesc);
	}

	vk::PipelineVertexInputStateCreateInfo vertInputStateInfo(
		{},										//flags
		1,										//vertexBindingDescriptionCount
		&inputBindingDescription,				//pVertexBindingDescriptions
		inputAttributeDescriptions.size(),		//vertexAttributeDescriptionCount
		inputAttributeDescriptions.data()		//pVertexAttributeDescriptions
	);

	// TODO: Make this customisable - tells the renderer what geometry to render, and whether we're reusing vertices or not
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo(
		{},										//flags
		vk::PrimitiveTopology::eTriangleList,	//topology
		vk::False								//primitiveRestartEnable
	);

	vk::Viewport viewport(
		0,					//x
		0,					//y
		scExtent.width,		//width
		scExtent.height,	//height
		0,					//minDepth
		1					//maxDepth
	);

	vk::Rect2D scissorRect(
		{0, 0},		//offset
		scExtent	//extent
	);

	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo(
		{},								//flags
		(uint32_t)dynamicStates.size(),	//dynamicStateCount
		dynamicStates.data()			//pDynamicStates
	);

	vk::PipelineViewportStateCreateInfo viewportStateInfo(
		{},			//flags
		1,			//viewportCount
		nullptr,	//pViewports | This will be assigned later, allowing us to have a dynamic viewport size
		1,			//scissorCount
		nullptr		//pScissors | As with pViewports, but for scissor rects instead
	);

	// TODO: Another reminder that pretty much everything in here should be customisable at some point
	// Almost all of these require certain GPU features to be enabled
	vk::PipelineRasterizationStateCreateInfo rasterisationStateInfo(
		{},								//flags
        vk::False,						//depthClampEnable
        vk::False,						//rasterizerDiscardEnable
		vk::PolygonMode::eFill,			//polygonMode
		vk::CullModeFlagBits::eBack,	//cullMode
		vk::FrontFace::eClockwise,		//frontFace
		vk::False,						//depthBiasEnable
		0,								//depthBiasConstantFactor
		0,								//depthBiasClamp
		0,								//depthBiasSlopeFactor
		1								//lineWidth
	);

	// TODO: Look into this at a later date
	vk::PipelineMultisampleStateCreateInfo multisampleStateInfo(
		{},								//flags
		vk::SampleCountFlagBits::e1,	//rasterizationSamples
		vk::False,						//sampleShadingEnable
		1,								//minSampleShading
		nullptr,						//pSampleMask
		vk::False,						//alphaToCoverageEnable
		vk::False						//alphaToOneEnable
	);

	// TODO: Proper blending stuff, more config
	// Regular blending method
	vk::PipelineColorBlendAttachmentState colourBlendAttachmentState(
		vk::True,															//blendEnable
		vk::BlendFactor::eSrcAlpha,											//srcColorBlendFactor
		vk::BlendFactor::eOneMinusSrcAlpha,									//dstColorBlendFactor
		vk::BlendOp::eAdd,													//colorBlendOp
		vk::BlendFactor::eOne,												//srcAlphaBlendFactor
		vk::BlendFactor::eZero,												//dstAlphaBlendFactor
		vk::BlendOp::eAdd,													//alphaBlendOp
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA		//colorWriteMask
	);

	// Bitwise blending method
	vk::PipelineColorBlendStateCreateInfo colourBlendStateInfo(
		{},								//flags
		vk::False,						//logicOpEnable
		vk::LogicOp::eCopy,				//logicOp
		1,								//attachmentCount
		&colourBlendAttachmentState,	//pAttachments
		{0, 0, 0, 0}					//blendConstants
	);

	// TODO: Empty for now, make this properly later
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
		{},			//flags
		0,			//setLayoutCount
		nullptr,	//pSetLayouts
		0,			//pushConstantRangeCount
		nullptr		//pPushConstantRanges
	);

	m_pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

	vk::GraphicsPipelineCreateInfo pipelineInfo(
		{},							//flags
		2,							//stageCount
		shaderStageInfos,			//pStages
		&vertInputStateInfo,		//pVertexInputState
		&inputAssemblyStateInfo,	//pInputAssemblyState
		nullptr,					//pTessellationState
		&viewportStateInfo,			//pViewportState
		&rasterisationStateInfo,	//pRasterizationState
		&multisampleStateInfo,		//pMultisampleState
		nullptr,					//pDepthStencilState
		&colourBlendStateInfo,		//pColorBlendState
		&dynamicStateInfo,			//pDynamicState
		m_pipelineLayout,			//layout
		m_renderPass,				//renderPass
		0,							//subpass
		nullptr,					//basePipelineHandle
		-1							//basePipelineIndex
	);

	vk::Result result;
	std::tie(result, m_graphicsPipeline) = device.createGraphicsPipeline(nullptr, pipelineInfo);
	if (result == vk::Result::ePipelineCompileRequired)
	{
		throw std::runtime_error("Creation of graphics pipeline failed with error code: " + std::to_string((int)result));
	}

	device.destroyShaderModule(vertShaderModule);
	device.destroyShaderModule(fragShaderModule);
}

void GraphicsPipelineWrapper::DestroyPipeline(vk::Device device)
{
	if (m_graphicsPipeline != nullptr) { device.destroyPipeline(m_graphicsPipeline); }
	if (m_pipelineLayout != nullptr) { device.destroyPipelineLayout(m_pipelineLayout); }
	if (m_renderPass != nullptr) { device.destroyRenderPass(m_renderPass); }
}
