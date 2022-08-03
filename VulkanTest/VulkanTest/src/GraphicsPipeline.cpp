#define VULKAN_HPP_NO_CONSTRUCTORS

#include "GraphicsPipeline.h"

#include "Vertex.h"

/**
 * \brief Creates a graphics pipeline
 * \param graphics_pipeline The vk::Pipeline object reference to be allocated.
 * \param pipeline_layout The vk::PipelineLayout object reference that is allocated and later used.
 * \param render_pass A render pass that determines coloring.
 * \param device The logical device that will handle object creations.
 * \param swap_chain_extent The extents of the current screen.
 * \param descriptor_set_layout The descriptor set layout used so that shaders know what uniforms to use.
 * \param shader The shaders that are being used for the pipeline.
 */
void GraphicsPipeline::CreateGraphicsPipeline(vk::Pipeline& graphics_pipeline,
                                              vk::PipelineLayout& pipeline_layout,
                                              vk::RenderPass render_pass,
                                              const vk::Device device,
                                              vk::Extent2D swap_chain_extent,
                                              const vk::DescriptorSetLayout descriptor_set_layout,
                                              const OpenGLShader& shader)
{
	vk::ShaderModule vert_shader_module = OpenGLShader::CreateShaderModule(device, shader,
	                                                                       vk::ShaderStageFlagBits::eVertex);
	vk::ShaderModule frag_shader_module = OpenGLShader::CreateShaderModule(device, shader,
	                                                                       vk::ShaderStageFlagBits::eFragment);

	vk::PipelineShaderStageCreateInfo vert_shader_stage_info{
		.stage = vk::ShaderStageFlagBits::eVertex,
		.module = vert_shader_module,
		.pName = "main"
	};

	vk::PipelineShaderStageCreateInfo frag_shader_stage_info{
		.stage = vk::ShaderStageFlagBits::eFragment,
		.module = frag_shader_module,
		.pName = "main"
	};

	vk::PipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

	auto binding_description = Vertex::GetBindingDescription();
	auto attribute_descriptions = Vertex::GetAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertex_input_info{
		.vertexBindingDescriptionCount = 1,
		// optional
		.pVertexBindingDescriptions = &binding_description,
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size()),
		.pVertexAttributeDescriptions = attribute_descriptions.data() // optional
	};

	// Viewports and scissors
	vk::PipelineInputAssemblyStateCreateInfo input_assembly{
		.topology = vk::PrimitiveTopology::eTriangleList,
		.primitiveRestartEnable = static_cast<vk::Bool32>(false)
	};

	vk::Viewport viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(swap_chain_extent.width),
		.height = static_cast<float>(swap_chain_extent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	vk::Rect2D scissor{
		.offset = {0, 0},
		.extent = swap_chain_extent
	};

	vk::PipelineViewportStateCreateInfo viewport_state{
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor
	};

	// Rasterizer
	vk::PipelineRasterizationStateCreateInfo rasterizer{
		.depthClampEnable = static_cast<vk::Bool32>(false),
		.rasterizerDiscardEnable = static_cast<vk::Bool32>(false),
		.polygonMode = vk::PolygonMode::eFill,
		.cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eCounterClockwise,
		.depthBiasEnable = static_cast<vk::Bool32>(false),
		// optional
		.depthBiasConstantFactor = 0.0f,
		// optional
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f
	};

	// Multi-sampling (temporarily disabled)
	vk::PipelineMultisampleStateCreateInfo multisampling{
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable = static_cast<vk::Bool32>(false),
		// optional
		.minSampleShading = 1.0f,
		// optional
		.pSampleMask = nullptr,
		// optional
		.alphaToCoverageEnable = static_cast<vk::Bool32>(false),
		// optional
		.alphaToOneEnable = static_cast<vk::Bool32>(false)
	};

	/* If depth or stencil testing is being done, vk::PipelineDepthStencilStateCreateInfo would be needed */

	// Color blending
	vk::PipelineColorBlendAttachmentState color_blend_attachment{
		.blendEnable = static_cast<vk::Bool32>(false),
		// optional
		.srcColorBlendFactor = vk::BlendFactor::eOne,
		// optional
		.dstColorBlendFactor = vk::BlendFactor::eZero,
		// optional
		.colorBlendOp = vk::BlendOp::eAdd,
		// optional
		.srcAlphaBlendFactor = vk::BlendFactor::eOne,
		// optional
		.dstAlphaBlendFactor = vk::BlendFactor::eZero,
		.alphaBlendOp = vk::BlendOp::eAdd,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
		                  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	};

	vk::PipelineColorBlendStateCreateInfo color_blending{
		.logicOpEnable = static_cast<vk::Bool32>(false),
		// optional
		.logicOp = vk::LogicOp::eCopy,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment,
		// optional
		.blendConstants = vk::ArrayWrapper1D<float, 4>{{0.0f, 0.0f, 0.0f, 0.0f}}
	};

	// Dynamic state
	std::vector dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eLineWidth};

	vk::PipelineDynamicStateCreateInfo dynamic_state{
		.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
		.pDynamicStates = dynamic_states.data()
	};

	// Pipeline layout
	vk::PipelineLayoutCreateInfo pipeline_layout_info{
		// optional
		.setLayoutCount = 1,
		// optional
		.pSetLayouts = &descriptor_set_layout,
		// optional
		.pushConstantRangeCount = 0,
		// optional
		.pPushConstantRanges = nullptr
	};

	// building the pipeline layout
	if (device.createPipelineLayout(&pipeline_layout_info, nullptr, &pipeline_layout) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create pipeline layout!");

	// Pipeline create info
	vk::GraphicsPipelineCreateInfo pipeline_info{
		.stageCount = 2,
		.pStages = shader_stages,
		.pVertexInputState = &vertex_input_info,
		.pInputAssemblyState = &input_assembly,
		.pViewportState = &viewport_state,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		// optional
		.pDepthStencilState = nullptr,
		.pColorBlendState = &color_blending,
		// optional
		.pDynamicState = nullptr,
		.layout = pipeline_layout,
		.renderPass = render_pass,
		.subpass = 0,
		// optional
		.basePipelineHandle = VK_NULL_HANDLE,
		// optional
		.basePipelineIndex = -1
	};

	if (device.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) !=
	    vk::Result::eSuccess)
		throw std::runtime_error("Failed to create graphics pipeline!");

	// End of Code
	// Since shader modules get converted to machine code, they can be destroyed
	device.destroyShaderModule(frag_shader_module, nullptr);
	device.destroyShaderModule(vert_shader_module, nullptr);
}

void GraphicsPipeline::CreateRenderPass(vk::RenderPass& render_pass,
                                        const vk::Device device,
                                        vk::Format swap_chain_image_format)
{
	// Attachment Description
	vk::AttachmentDescription color_attachment{
		.format = swap_chain_image_format,
		.samples = vk::SampleCountFlagBits::e1,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
		.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		.initialLayout = vk::ImageLayout::eUndefined,
		.finalLayout = vk::ImageLayout::ePresentSrcKHR
	};

	// Subpasses and attachment references
	vk::AttachmentReference color_attachment_ref{
		.attachment = 0,
		.layout = vk::ImageLayout::eColorAttachmentOptimal
	};

	vk::SubpassDescription subpass{
		.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_ref
	};

	vk::SubpassDependency dependency{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.srcAccessMask = vk::AccessFlagBits::eNone,
		.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
	};

	// Render pass
	vk::RenderPassCreateInfo render_pass_info{
		.attachmentCount = 1,
		.pAttachments = &color_attachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};

	if (device.createRenderPass(&render_pass_info, nullptr, &render_pass) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create render pass!");
}
