#ifndef __SCOPE_RENDERER_MESH__
#define __SCOPE_RENDERER_MESH__

#include <vector>
#include <cstdint>
#include <cstring>

#include <Renderer/Vertex.h>
#include <Renderer/Buffer.h>
#include <Utils/Buffer.h>

namespace Scop
{
	namespace Internal
	{
		[[nodiscard]] inline bool IsPowerOfTwo(int n) noexcept
		{
			if(n == 0)
				return false;
			return (std::ceil(std::log2(n)) == std::floor(std::log2(n)));
		}
		
		[[nodiscard]] inline std::size_t UpperPowerOfTwo(std::size_t v)
		{
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;
			v++;
			return v;
		}
	}

	class Mesh
	{
		public:
			struct SubMesh
			{
				VertexBuffer vbo;
				IndexBuffer ibo;
				std::size_t triangle_count = 0;

				inline SubMesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
				{
					CPUBuffer vb(vertices.size() * sizeof(Vertex));
					std::memcpy(vb.GetData(), vertices.data(), vb.GetSize());
					std::size_t vb_size = vb.GetSize();
					//if(!Internal::IsPowerOfTwo(vb_size))
					//	vb_size = Internal::UpperPowerOfTwo(vb_size);
					vbo.Init(vb_size);
					vbo.SetData(std::move(vb));

					CPUBuffer ib(indices.size() * sizeof(std::uint32_t));
					std::memcpy(ib.GetData(), indices.data(), ib.GetSize());
					std::size_t ib_size = ib.GetSize();
					//if(!Internal::IsPowerOfTwo(ib_size))
					//	ib_size = Internal::UpperPowerOfTwo(ib_size);
					ibo.Init(ib_size);
					ibo.SetData(std::move(ib));

					triangle_count = vertices.size() / 3;
				}
			};

		public:
			Mesh() = default;

			void Draw(VkCommandBuffer cmd, std::size_t& drawcalls, std::size_t& polygondrawn) const noexcept;
			void Draw(VkCommandBuffer cmd, std::size_t& drawcalls, std::size_t& polygondrawn, std::size_t submesh_index) const noexcept;

			inline std::size_t GetSubMeshCount() const { return m_sub_meshes.size(); }

			inline void AddSubMesh(SubMesh mesh) { m_sub_meshes.emplace_back(std::move(mesh)); }

			~Mesh();

		private:
			std::vector<SubMesh> m_sub_meshes;
	};
}

#endif
