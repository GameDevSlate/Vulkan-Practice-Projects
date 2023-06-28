﻿#define VULKAN_HPP_NO_CONSTRUCTORS

#pragma once
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

struct Vertex
{
	glm::vec2 pos;

	glm::vec3 color;

	glm::vec2 texCoord;

	static vk::VertexInputBindingDescription GetBindingDescription()
	{
		vk::VertexInputBindingDescription binding_description;
		binding_description.binding = 0;
		binding_description.stride = sizeof(Vertex);
		binding_description.inputRate = vk::VertexInputRate::eVertex;

		return binding_description;
	}

	static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 3> attribute_descriptions{};

		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].format = vk::Format::eR32G32Sfloat;
		attribute_descriptions[0].offset = offsetof(Vertex, pos);

		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attribute_descriptions[1].offset = offsetof(Vertex, color);

		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].format = vk::Format::eR32G32Sfloat;
		attribute_descriptions[2].offset = offsetof(Vertex, texCoord);

		return attribute_descriptions;
	}
};
