#ifndef __SCOP_SHADER__
#define __SCOP_SHADER__

#include <vector>
#include <cstdint>
#include <filesystem>
#include <unordered_map>

#include <kvf.h>

namespace Scop
{
	struct ShaderSetLayout
	{
		std::vector<std::pair<int, VkDescriptorType> > binds;

		ShaderSetLayout(std::vector<std::pair<int, VkDescriptorType> > b) : binds(std::move(b)) {}
	};

	struct ShaderPushConstantLayout
	{
		std::size_t offset;
		std::size_t size;

		ShaderPushConstantLayout(std::size_t o, std::size_t s) : offset(o), size(s) {}
	};

	struct ShaderLayout
	{
		std::vector<std::pair<int, ShaderSetLayout> > set_layouts;
		std::vector<ShaderPushConstantLayout> push_constants;

		ShaderLayout(std::vector<std::pair<int, ShaderSetLayout> > s, std::vector<ShaderPushConstantLayout> pc) : set_layouts(std::move(s)), push_constants(std::move(pc)) {}
	};

	enum class ShaderType
	{
		Vertex,
		Fragment,
		Compute
	};

	class Shader
	{
		public:
			Shader(const std::vector<std::uint32_t>& bytecode, ShaderType type, ShaderLayout layout);

			inline const std::vector<std::uint32_t>& GetByteCode() const noexcept { return m_bytecode; }
			inline VkPipelineLayout GetPipelineLayout() const noexcept { return m_pipeline_layout; }
			inline VkDescriptorSet GetDescriptorSet(std::size_t n) const { return m_sets.at(n); }
			inline VkShaderModule GetShaderModule() const noexcept { return m_module; }

			void LoadDescriptorSets() noexcept;

			~Shader();

		private:
			void GeneratePipelineLayout(ShaderLayout layout);

		private:
			std::vector<std::uint32_t> m_bytecode;
			std::unordered_map<std::size_t, VkDescriptorSet> m_sets;
			std::vector<VkDescriptorSetLayout> m_set_layouts;
			VkShaderStageFlagBits m_stage;
			VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
			VkShaderModule m_module = VK_NULL_HANDLE;
	};

	std::shared_ptr<Shader> LoadShaderFromFile(const std::filesystem::path& filepath, ShaderType type, ShaderLayout layout);
}

#endif
