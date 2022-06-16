#include "ShaderLibrary.h"
#include "Core/Assert.h"

void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
{
	VK_CORE_ASSERT(!Exists(name), "Shader already exists!");
	m_shaders[name] = shader;
}

void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
{
	auto& name = shader->GetName();
	Add(name, shader);
}

std::shared_ptr<Shader> ShaderLibrary::Load(vk::Device device, const std::string& file_path)
{
	auto shader = Shader::Create(file_path);
	Add(shader);
	return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Load(vk::Device device, const std::string& name, const std::string& file_path)
{
	auto shader = Shader::Create(file_path);
	Add(name, shader);
	return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
{
	VK_CORE_ASSERT(Exists(name), "Shader not found!");
	return m_shaders[name];
}

bool ShaderLibrary::Exists(const std::string& name) const
{
	return m_shaders.contains(name);
}
