#include <algorithm>

#include <Renderer/Pipelines/Shader.h>
#include <Renderer/ShadersLibrary.h>
#include <Core/Logs.h>

namespace Scop
{
	std::shared_ptr<Shader> ShadersLibrary::GetShader(ShaderID id)
	{
		if(!m_cache.count(id))
			FatalError("Shaders Library : wrong shader ID '%'", id);
		return m_cache[id];
	}

	ShaderID ShadersLibrary::AddShaderToLibrary(std::shared_ptr<Shader> shader)
	{
		auto it = std::find_if(m_cache.begin(), m_cache.end(), [&](const std::pair<ShaderID, std::shared_ptr<Shader>>& cached_shader)
		{
			return cached_shader.second->GetByteCode() == shader->GetByteCode();
		});
		if(it != m_cache.end())
			return it->first;
		m_cache[m_current_id] = shader;
		m_current_id++;
		return m_current_id - 1;
	}

	void ShadersLibrary::RemoveShaderFromLibrary(ShaderID id)
	{
		if(!m_cache.count(id))
		{
			Warning("Shader Library : trying to remove a shader with an unkown or invalid ID '%'", id);
			return;
		}
	}

	void ShadersLibrary::ClearLibrary()
	{
		m_cache.clear();
	}
}
