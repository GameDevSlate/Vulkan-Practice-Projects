#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

#include "Shader.h"

class ShaderLibrary
{
public:
	void Add(const std::string& name, const std::shared_ptr<Shader>& shader);

	void Add(const std::shared_ptr<Shader>& shader);

	std::shared_ptr<Shader> Load(vk::Device device, const std::string& file_path);

	std::shared_ptr<Shader> Load(vk::Device device, const std::string& name, const std::string& file_path);

	std::shared_ptr<Shader> Get(const std::string& name);

	[[nodiscard]] bool Exists(const std::string& name) const;

private:
	std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
};
