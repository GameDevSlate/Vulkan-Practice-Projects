#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "Shader.h"

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& file_path);

	OpenGLShader(const std::string& name,
	             const std::string& vertex_src,
	             const std::string& fragment_src);

	~OpenGLShader() override;

	void Bind() const override;

	void Unbind() const override;

	void SetInt(const std::string& name, int value) override;

	void SetIntArray(const std::string& name, int* values, uint32_t count) override;

	void SetFloat(const std::string& name, float value) override;

	void SetFloat2(const std::string& name, const glm::vec2& value) override;

	void SetFloat3(const std::string& name, const glm::vec3& value) override;

	void SetFloat4(const std::string& name, const glm::vec4& value) override;

	void SetMat4(const std::string& name, const glm::mat4& value) override;

	[[nodiscard]] const std::string& GetName() const override { return m_name; }

	void UploadUniformInt(const std::string& name, float value);

	void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

	void UploadUniformFloat(const std::string& name, float value);

	void UploadUniformFloat2(const std::string& name, const glm::vec2& value);

	void UploadUniformFloat3(const std::string& name, const glm::vec3& value);

	void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);

	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

private:
	static std::string Readfile(const std::string& file_path);

	static std::unordered_map<vk::ShaderStageFlagBits, std::string> PreProcess(const std::string& source);

	void CompileOrGetVulkanBinaries(const std::unordered_map<vk::ShaderStageFlagBits, std::string>& shader_sources);

	static void CompileOrGetOpenGLBinaries();

	static void CreateShaderModule(vk::Device device);

	void Reflect(vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& shader_data);

private:
	uint32_t m_rendererID;

	std::unordered_map<vk::ShaderStageFlagBits, std::string> m_filePaths;

	std::string m_name;

	std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>> m_vulkanSpirv;

	std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>> m_openGLSPIRV;

	std::unordered_map<vk::ShaderStageFlagBits, std::string> m_openGLSourceCode;
};
