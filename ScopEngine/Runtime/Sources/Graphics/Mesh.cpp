#include <Graphics/Mesh.h>
#include <Utils/Buffer.h>
#include <cstring>

namespace Scop
{
	void Mesh::Draw(VkCommandBuffer cmd, std::size_t& drawcalls) const noexcept
	{
		for(std::size_t i = 0; i < m_sub_meshes.size(); i++)
			Draw(cmd, drawcalls, i);
	}

	void Mesh::Draw(VkCommandBuffer cmd, std::size_t& drawcalls, std::size_t submesh_index) const noexcept
	{
		Verify(submesh_index < m_sub_meshes.size(), "invalid submesh index");
		m_sub_meshes[submesh_index].vbo.Bind(cmd);
		m_sub_meshes[submesh_index].ibo.Bind(cmd);
		vkCmdDrawIndexed(cmd, static_cast<std::uint32_t>(m_sub_meshes[submesh_index].ibo.GetSize() / sizeof(std::uint32_t)), 1, 0, 0, 0);
		drawcalls++;
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