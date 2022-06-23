#define VULKAN_HPP_NO_CONSTRUCTORS

#include "OpenGLShader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Core/Assert.h"
#include "Core/Log.h"
#include "Core/Timer.h"

class ShaderUtils
{
public:
	/**
	 * \brief Takes in a string literal, and returns the the enum shader stage that it is.
	 * \param type The string depicting the type of shader that the function looks for.
	 * \return A shader type flag bit.
	 */
	static vk::ShaderStageFlagBits ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return vk::ShaderStageFlagBits::eVertex;
		if (type == "fragment" || type == "pixel")
			return vk::ShaderStageFlagBits::eFragment;

		VK_CORE_ASSERT(false, "Unknown shader type!");

		throw std::runtime_error("ERROR!");
	}

	/**
	 * \brief A conversion from a vulkan shader stage, to a shaderc shader stage.
	 * \param stage the vulkan shader stage enum.
	 * \return The shaderc shader stage enum.
	 */
	static shaderc_shader_kind GLShaderStageToShaderC(const vk::ShaderStageFlagBits stage)
	{
		switch (stage) {
			case vk::ShaderStageFlagBits::eVertex:
				return shaderc_glsl_vertex_shader;

			case vk::ShaderStageFlagBits::eFragment:
				return shaderc_glsl_fragment_shader;

			default:
				VK_CORE_ASSERT(false);
				return static_cast<shaderc_shader_kind>(0);

		}
	}

	/**
	 * \brief Converts a vulkan shader stage enum to a string.
	 * \param stage The vulkan shader flag bit stage enum.
	 * \return The string literal.
	 */
	static const char* GLShaderStageToString(const vk::ShaderStageFlagBits stage)
	{
		switch (stage) {

			case vk::ShaderStageFlagBits::eVertex:
				return "GLSL_VERTEX_SHADER";

			case vk::ShaderStageFlagBits::eFragment:
				return "GLSL_FRAGMENT_SHADER";

			default:
				VK_CORE_ASSERT(false);
				return nullptr;
		}
	}

	static const char* GetCacheDirectory()
	{
		return "assets/cache/shaders/glsl";
	}

	/**
	 * \brief If there is no previous cache of shaders, create a new one for future use.
	 */
	static void CreateCacheDirectoryIfNeeded()
	{
		std::string cache_directory = GetCacheDirectory();

		if (!std::filesystem::exists(cache_directory))
			std::filesystem::create_directories(cache_directory);
	}

	static const char* GLShaderStageCacheOpenGLFileExtension(const vk::ShaderStageFlagBits stage)
	{
		switch (stage) {
			case vk::ShaderStageFlagBits::eVertex:
				return ".cached_glsl.vert";

			case vk::ShaderStageFlagBits::eFragment:
				return ".cached_glsl.frag";

			default:
				VK_ASSERT(false);
				return "";
		}
	}

	/**
	 * \brief Converts a vulkan shader stage enum to a corresponding file type ending string.
	 * \param stage The type of shader stage enum.
	 * \return The file name extension string.
	 */
	static const char* GLShaderStageCachedVulkanFileExtension(const vk::ShaderStageFlagBits stage)
	{
		switch (stage) {
			case vk::ShaderStageFlagBits::eVertex:
				return ".cached_vulkan.vert";

			case vk::ShaderStageFlagBits::eFragment:
				return ".cached_vulkan.frag";

			default:
				VK_ASSERT(false);
				return "";
		}
	}
};

/**
 * \brief Creates a shader with on single file path.
 * \param file_path The file path of the shader.
 */
OpenGLShader::OpenGLShader(const std::string& file_path)
{
	/*
	 * Since this shader only uses a single file path, it will set
	 * all file locations in its hash directory to be the same.
	 */
	m_filePaths[vk::ShaderStageFlagBits::eVertex] = file_path;
	m_filePaths[vk::ShaderStageFlagBits::eFragment] = file_path;

	ShaderUtils::CreateCacheDirectoryIfNeeded();

	std::string source = Readfile(file_path);

	/*
	 * Since this is a single file path, parse the file to see if there are
	 * multiple shaders in it.
	 */
	auto shader_sources = PreProcess(source);

	// This empty scope is for calculating the amount it takes to compile all shaders
	{
		Timer timer;

		CompileOrGetVulkanBinaries(shader_sources);
		//CompileOrGetOpenGLBinaries();
		// CreateShaderModule(device);
		VK_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
	}

	// Extract name from file_path
	auto last_slash = file_path.find_last_of("/\\");
	last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
	auto last_dot = file_path.rfind('.');
	auto count = last_dot == std::string::npos ? file_path.size() - last_slash : last_dot - last_slash;
	m_name = file_path.substr(last_slash, count);
}

/**
 * \brief Creates a shader from a separate vertex and fragment shader files.
 * \param name The assigned name for the shader.
 * \param vertex_src The vertex shader source code file location.
 * \param fragment_src The fragment shader source code file location.
 */
OpenGLShader::OpenGLShader(const std::string& name,
                           const std::string& vertex_src,
                           const std::string& fragment_src) : m_name(name)
{
	// Assign the corresponding source code directories.
	m_filePaths[vk::ShaderStageFlagBits::eVertex] = vertex_src;
	m_filePaths[vk::ShaderStageFlagBits::eFragment] = fragment_src;

	ShaderUtils::CreateCacheDirectoryIfNeeded();

	// Read the source code from each directory provided.
	std::unordered_map<vk::ShaderStageFlagBits, std::string> sources;
	sources[vk::ShaderStageFlagBits::eVertex] = Readfile(vertex_src);
	sources[vk::ShaderStageFlagBits::eFragment] = Readfile(fragment_src);

	{
		Timer timer;

		CompileOrGetVulkanBinaries(sources);
		//CompileOrGetOpenGLBinaries();
		//CreateShaderModule(device);
		//CompileOrGetOpenGLBinaries();
		//CreateShaderModule(device);
		VK_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
	}
}

OpenGLShader::~OpenGLShader() {}

/**
 * \brief Creates a shader module
 * \param device The logical device used to create the shader module.
 * \param shader The shader object that holds the vulkan byte code of the type of shader.
 * \param stage The type of shader stage that should be created.
 * \return A new shader module of the type of stage it was designated to be.
 */
vk::ShaderModule OpenGLShader::CreateShaderModule(const vk::Device device,
                                                  OpenGLShader shader,
                                                  const vk::ShaderStageFlagBits stage)
{
	vk::ShaderModuleCreateInfo create_info{
		/*
		 * Since the data store inside the vulkan spirv binaries is an array
		 * of unsigned integers, when getting the size of the actual data,
		 * it has to be multiplied by the size of uint32_t, which is the same.
		 */
		.codeSize = shader.m_vulkanSpirv[stage].size() * sizeof(uint32_t),
		.pCode = shader.m_vulkanSpirv[stage].data()
	};

	vk::ShaderModule shader_module;

	if (device.createShaderModule(&create_info, nullptr, &shader_module) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create shader module! of type: " + static_cast<std::string>(
			                         ShaderUtils::GLShaderStageToString(stage)));

	return shader_module;
}

void OpenGLShader::Bind() const {}

void OpenGLShader::Unbind() const {}

void OpenGLShader::SetInt(const std::string& name, int value) {}

void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count) {}

void OpenGLShader::SetFloat(const std::string& name, float value) {}

void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value) {}

void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value) {}

void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value) {}

void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {}

/**
 * \brief Attempts to open a file at a provided directory.
 * \param file_path The file location directory.
 * \return A string containing the text from inside the file.
 */
std::string OpenGLShader::Readfile(const std::string& file_path)
{
	std::string result;

	if (std::ifstream in(file_path, std::ios::in | std::ios::binary); in) {

		in.seekg(0, std::ios::end);
		size_t size = in.tellg();

		if (size != -1) {

			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(&result[0], size);
		} else
			VK_CORE_ERROR("Could not read from file '{0}'", file_path);
	} else
		VK_CORE_ERROR("Could not open file '{0}'", file_path);

	return result;
}

/**
 * \brief Parses a single shader source code file.
 * \param source The source code of the file.
 * \return The separated shaders from inside the original file.
 */
std::unordered_map<vk::ShaderStageFlagBits, std::string> OpenGLShader::PreProcess(const std::string& source)
{
	std::unordered_map<vk::ShaderStageFlagBits, std::string> shader_sources;

	auto type_token = "#type";

	size_t type_token_length = strlen(type_token);
	size_t pos = source.find(type_token, 0); // Start of shader type declaration line

	while (pos != std::string::npos) {

		size_t eol = source.find_first_of("\r\n", pos); // End of shader type declaration line

		VK_CORE_ASSERT(eol != std::string::npos, "Syntax error");

		size_t begin = pos + type_token_length + 1; // Start of shader type name (after "#type" keyword)
		std::string type = source.substr(begin, eol - begin);

		const bool type_condition = (ShaderUtils::ShaderTypeFromString(type) == vk::ShaderStageFlagBits::eVertex) || (
			                            ShaderUtils::ShaderTypeFromString(type) == vk::ShaderStageFlagBits::eFragment);

		VK_CORE_ASSERT(type_condition, "Invalid shader type specified");

		size_t next_line_pos = source.find_first_not_of("\r\n", eol);
		// Start of shader code after shader type declaration line

		VK_CORE_ASSERT(next_line_pos != std::string::npos, "Syntax error");
		pos = source.find(type_token, next_line_pos); // Start of next shader type declaration line

		shader_sources[ShaderUtils::ShaderTypeFromString(type)] = (pos == std::string::npos) ?
			                                                          source.substr(next_line_pos) :
			                                                          source.substr(next_line_pos, pos - next_line_pos);
	}

	return shader_sources;
}

/**
 * \brief Compiles GLSL shader to vulkan SPIRV binaries.
 * \param shader_sources The list of each shader stage, along with its source code.
 */
void OpenGLShader::CompileOrGetVulkanBinaries(
	const std::unordered_map<vk::ShaderStageFlagBits, std::string>& shader_sources)
{
	// Creating a compiler and its options
	shaderc::CompileOptions options;

	options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

	const bool optimize = true;
	if (optimize)
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

	std::filesystem::path cache_directory = ShaderUtils::GetCacheDirectory();

	auto& shader_data = m_vulkanSpirv;
	shader_data.clear();

	// Read each shader from their source codes
	for (auto&& [stage, source] : shader_sources) {

		std::filesystem::path shader_file_path = m_filePaths[stage];
		std::filesystem::path cached_path =
			cache_directory / (shader_file_path.filename().string().
			                                    substr(0, shader_file_path.filename().string().find_first_of('.')) +
			                   ShaderUtils::GLShaderStageCachedVulkanFileExtension(stage));

		// Create an input file stream to see if the source file
		// that is being looked upon is open (meaning that it also exists),
		// so no compilation has to be done
		if (std::ifstream in(cached_path, std::ios::in | std::ios::binary); in.is_open()) {
			in.seekg(0, std::ios::end);
			auto size = in.tellg();
			in.seekg(0, std::ios::beg);

			auto& data = shader_data[stage];
			data.resize(size / sizeof(uint32_t));
			in.read(reinterpret_cast<char*>(data.data()), size);
		} else {

			// Create a compiler
			shaderc::Compiler compiler;

			// Convert the glsl code to SPIRV byte code.
			shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source,
			                                                                 ShaderUtils::GLShaderStageToShaderC(stage),
			                                                                 m_filePaths[stage].c_str(), options);
			// Check that the compilation was successful.
			if (module.GetCompilationStatus() != shaderc_compilation_status_success) {

				VK_CORE_ERROR(module.GetErrorMessage());
				VK_CORE_ASSERT(false);
			}

			// Get the byte code from the compilation.
			shader_data[stage] = std::vector(module.cbegin(), module.cend());

			if (std::ofstream out(cached_path, std::ios::out | std::ios::binary); out.is_open()) {

				auto& data = shader_data[stage];
				out.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(uint32_t));
				out.flush();
				out.close();
			}
		}
	}

	for (auto&& [stage, data] : shader_data)
		Reflect(stage, data);
}

void OpenGLShader::CompileOrGetOpenGLBinaries() {}

void OpenGLShader::CreateShaderModule(vk::Device device) {}

/**
 * \brief Reflects the information of a compiled shader.
 * \param stage The type of shader stage.
 * \param shader_data The byte code of the shader.
 */
void OpenGLShader::Reflect(const vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& shader_data)
{
	spirv_cross::Compiler compiler(shader_data);
	spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	VK_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", ShaderUtils::GLShaderStageToString(stage), m_filePaths[stage]);
	VK_CORE_TRACE("\t{0} uniform buffers", resources.uniform_buffers.size());
	VK_CORE_TRACE("\t{0} resources", resources.sampled_images.size());

	VK_CORE_TRACE("Uniform buffers:");

	for (const auto& resource : resources.uniform_buffers) {

		const auto& buffer_type = compiler.get_type(resource.base_type_id);
		uint32_t buffer_size = compiler.get_declared_struct_size(buffer_type);
		uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
		int member_count = buffer_type.member_types.size();

		VK_CORE_TRACE("\t{0}", resource.name);
		VK_CORE_TRACE("\tSize = {0}", buffer_size);
		VK_CORE_TRACE("\tBinding = {0}", binding);
		VK_CORE_TRACE("\tMembers = {0}", member_count);
	}
}
