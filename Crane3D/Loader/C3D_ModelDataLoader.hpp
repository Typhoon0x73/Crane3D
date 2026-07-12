# pragma once
# include "C3D_ResourceLoader.hpp"
# include "../Define/C3D_ModelData.hpp"

namespace crane
{

	class ModelDataLoader : public ResourceLoader
	{
	public:

		explicit ModelDataLoader(const FilePath& path, ModelData& out, const LoadAnimeOnlyData animeOnly);

		virtual void loadAsync() override;

		static LoadStatus Load(const FilePath& path, ModelData& out, const LoadAnimeOnlyData animeOnly);

	private:

		FilePath m_filePath;
		ModelData& m_refModelData;
		LoadAnimeOnlyData m_animeOnly;

	};
}
