# pragma once
# include <Siv3D.hpp>
# include "../Define/AssimpWrapper.hpp"
# include "../Define/C3D_ModelData.hpp"

// ダンプ用
// # define CRANE3D_USE_DUMP_MODEL_DATA
// # define CRANE3D_USE_DUMP_ASSIMP_DATA

namespace crane
{

# if SIV3D_BUILD(DEBUG)

	struct Dump
	{
		static void AssimpScene(const aiScene* pScene, const FilePathView path);
		static void ModelData(const ModelData& data, const FilePathView path);
	};
# endif // SIV3D_BUILD(DEBUG)

}
