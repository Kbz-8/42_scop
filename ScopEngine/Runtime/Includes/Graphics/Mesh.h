#ifndef __SCOPE_RENDERER_MESH__
#define __SCOPE_RENDERER_MESH__

#include <vector>

#include <Renderer/Vertex.h>
#include <Renderer/Buffer.h>

namespace Scop
{
	class Mesh
	{
		public:
			struct SubMesh
			{
				VertexBuffer vbo;
				IndexBuffer ibo;
			};

		public:
			Mesh() = default;

			void Draw(VkCommandBuffer cmd, std::size_t& drawcalls) noexcept;

			inline void AddSubMesh(SubMesh mesh) { m_sub_meshes.emplace_back(std::move(mesh)); }

			~Mesh();

		private:
			std::vector<SubMesh> m_sub_meshes;
	};
}

#endif
