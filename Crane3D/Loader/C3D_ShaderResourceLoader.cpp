# include "C3D_ShaderResourceLoader.hpp"

namespace crane
{
	ShaderResourceLoader::ShaderResourceLoader(
		const ShaderStage stage
		, const NameHash hash
		, const ShaderGroup& group
		, ShaderResource& out
	)
		: ResourceLoader{}
		, m_stage{ stage }
		, m_nameHash{ hash }
		, m_shaderGroup{ group }
		, m_refShaderResource{ out }
	{
	}

	void ShaderResourceLoader::loadAsync()
	{
		m_optLoadTask.reset();
		m_optLoadTask = Async(Load, m_stage, m_nameHash, std::ref(m_shaderGroup), std::ref(m_refShaderResource));
	}

	LoadStatus ShaderResourceLoader::Load(
		const ShaderStage stage
		, const NameHash hash
		, const ShaderGroup& group
		, ShaderResource& out
	)
	{
		if (stage == ShaderStage::Vertex)
		{
			if (out.vsTable.contains(hash))
			{
				return LoadStatus::Succeeded;
			}
			VertexShader vs = group;
			if (not vs)
			{
				return LoadStatus::Failed;
			}
			out.vsTable[hash] = std::move(vs);
		}
		else if (stage == ShaderStage::Pixel)
		{
			if (out.psTable.contains(hash))
			{
				return LoadStatus::Succeeded;
			}
			PixelShader ps = group;
			if (not ps)
			{
				return LoadStatus::Failed;
			}
			out.psTable[hash] = std::move(ps);
		}
		return LoadStatus::Succeeded;
	}

}
