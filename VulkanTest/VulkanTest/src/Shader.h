#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

class Shader
{
public:
	virtual ~Shader() = default;

	virtual void Bind() const = 0;

	virtual void Unbind() const = 0;

	virtual void SetInt(const std::string& name, int value) = 0;

	virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;

	virtual void SetFloat(const std::string& name, float value) = 0;

	virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;

	virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;

	virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;

	virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

	[[nodiscard]] virtual const std::string& GetName() const = 0;

	static std::shared_ptr<Shader> Create(const std::string& filepath);

	static std::shared_ptr<Shader> Create(
		const std::string& name,
		const std::string& vertex_src,
		const std::string& fragment_src);
};
