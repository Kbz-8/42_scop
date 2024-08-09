#include <Graphics/Mesh.h>
#include <Utils/Buffer.h>
#include <cstring>

namespace Scop
{
	void Mesh::Draw(VkCommandBuffer cmd, std::size_t& drawcalls) noexcept
	{
		for(auto& mesh : m_sub_meshes)
		{
			mesh.vbo.Bind(cmd);
			mesh.ibo.Bind(cmd);
			vkCmdDrawIndexed(cmd, static_cast<std::uint32_t>(mesh.ibo.GetSize() / sizeof(std::uint32_t)), 1, 0, 0, 0);
			drawcalls++;
		}
	}

	Mesh::~Mesh()
	{
		for(auto& mesh : m_sub_meshes)
		{
			mesh.vbo.Destroy();
			mesh.ibo.Destroy();
		}
	}
}
