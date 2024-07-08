#include <Renderer/Pipelines/Shader.h>
#include <Renderer/RenderCore.h>
#include <Core/Logs.h>
#include <fstream>

namespace Scop
{
	Shader::Shader(const std::vector<std::uint32_t>& bytecode, ShaderType type, ShaderLayout layout) : m_bytecode(bytecode)
	{
		switch(type)
		{
			case ShaderType::Vertex : m_stage = VK_SHADER_STAGE_VERTEX_BIT; break;
			case ShaderType::Fragment : m_stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
			case ShaderType::Compute : m_stage = VK_SHADER_STAGE_COMPUTE_BIT; break;

			default : break;
		}

		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = m_bytecode.size() * sizeof(std::uint32_t);
		create_info.pCode = m_bytecode.data();

		if(vkCreateShaderModule(RenderCore::Get().GetDevice(), &create_info, nullptr, &m_module) != VK_SUCCESS)
			FatalError("Vulkan : failed to create shader module");
		Message("Vulkan : created shader module");

		GeneratePipelineLayout(layout);
	}

	void Shader::GeneratePipelineLayout(ShaderLayout layout)
	{
	}

	Shader::~Shader()
	{
		vkDestroyShaderModule(RenderCore::Get().GetDevice(), m_module, nullptr);
	}

	std::shared_ptr<Shader> LoadShaderFromFile(const std::filesystem::path& filepath, ShaderType type, ShaderLayout layout)
	{
		std::ifstream stream(filepath, std::ios::binary);
		if(!stream.is_open())
			FatalError("Renderer : unable to open a spirv shader file, %", filepath);
		std::vector<std::uint32_t> data;
		stream.seekg(0);
		std::uint32_t part = 0;
		while(stream.read(reinterpret_cast<char*>(&part), sizeof(part)))
			data.push_back(part);
		stream.close();

		std::shared_ptr<Shader> shader = std::make_shared<Shader>(data, type, layout);
		return shader;
	}
}
