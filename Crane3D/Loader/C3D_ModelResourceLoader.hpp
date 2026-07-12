# pragma once
# include "C3D_ResourceLoader.hpp"
# include "C3D_LoadInfo.hpp"
# include "../Define/C3D_ModelData.hpp"
# include "../Define/C3D_ModelResource.hpp"

namespace crane
{

	class ModelResourceLoader : public ResourceLoader
	{
	public:

		explicit ModelResourceLoader(ModelData& data, const LoadInfo& loadInfo, ModelResource& out);
		virtual void loadAsync() override;
		static LoadStatus Load(ModelData& data, const LoadInfo& addInfo, ModelResource& out);

	private:

		ModelData& m_refData;
		LoadInfo m_loadInfo;
		ModelResource& m_refResource;

	};
}
