#pragma once

#include "OpenGLShader.h"

class GraphicsPipeline
{
public:
	static void CreateGraphicsPipeline(vk::PipelineLayout& pipeline_layout,
	                                   vk::Device device,
	                                   vk::Extent2D swap_chain_extent,
	                                   const OpenGLShader& shader);

	static void CreateRenderPass(vk::RenderPass& render_pass, vk::Device device, vk::Format swap_chain_image_format);
};
