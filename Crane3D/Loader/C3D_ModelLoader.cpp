# include "C3D_ModelLoader.hpp"
# include "C3D_ModelDataLoader.hpp"
# include "C3D_ModelResourceLoader.hpp"

namespace crane
{
	ModelLoader::ModelLoader(const LoadInfo& info, crane::Model& out)
		: m_loadInfo{ info }
		, m_refModel{ out }
	{
	}

	void ModelLoader::loadAsync()
	{
		m_optLoadTask.reset();
		m_optLoadTask = Async(Load, std::ref(m_loadInfo), std::ref(m_refModel));
	}

	LoadStatus ModelLoader::Load(const LoadInfo& info, crane::Model& out)
	{
		auto loadResult = ModelDataLoader::Load(info.mainPath, out.data, LoadAnimeOnlyData::No);
		if (loadResult != LoadStatus::Succeeded)
		{
			return LoadStatus::Failed;
		}

		for (const auto& path : info.animePaths)
		{
			loadResult = ModelDataLoader::Load(path, out.data, LoadAnimeOnlyData::Yes);
			if (loadResult != LoadStatus::Succeeded)
			{
				return LoadStatus::Failed;
			}
		}

		auto createResult = ModelResourceLoader::Load(out.data, info, out.resource);
		if (createResult != LoadStatus::Succeeded)
		{
			return LoadStatus::Failed;
		}

		return LoadStatus::Succeeded;
	}

}
