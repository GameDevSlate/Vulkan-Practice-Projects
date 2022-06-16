#include "Shader.h"
#include "OpenGLShader.h"

std::shared_ptr<Shader> Shader::Create(const std::string& filepath)
{
	return std::make_shared<OpenGLShader>(filepath);
}

std::shared_ptr<Shader> Shader::Create(
	const std::string& name,
	const std::string& vertex_src,
	const std::string& fragment_src)
{
	return std::make_shared<OpenGLShader>(name, vertex_src, fragment_src);
}
