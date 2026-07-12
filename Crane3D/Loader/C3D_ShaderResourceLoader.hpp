# pragma once
# include "C3D_ResourceLoader.hpp"
# include "../Define/C3D_Define.hpp"
# include "../Define/C3D_ShaderResource.hpp"

namespace crane
{

	class ShaderResourceLoader : public ResourceLoader
	{
	public:

		explicit ShaderResourceLoader(
			const ShaderStage stage, const NameHash hash
			, const ShaderGroup& group, ShaderResource& out
		);

		virtual void loadAsync() override;

		static LoadStatus Load(
			const ShaderStage stage, const NameHash hash
			, const ShaderGroup& group, ShaderResource& out
		);

	private:

		ShaderStage m_stage;
		NameHash m_nameHash;
		ShaderGroup m_shaderGroup;
		ShaderResource& m_refShaderResource;
	};
}
