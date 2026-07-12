# pragma once
# include <Siv3D.hpp>
# include "../Define/AssimpWrapper.hpp"

namespace crane
{
	constexpr uint32 LoadFlagDefault =
		aiProcess_MakeLeftHanded
		| aiProcess_FlipWindingOrder
		| aiProcess_LimitBoneWeights
		| aiProcess_FlipUVs
		| aiProcess_Triangulate
		| aiProcess_SortByPType
		;

	enum class LoadStatus
	{
		NonTask = -2,
		NowLoading = -1,
		Succeeded = 0,
		Failed = 1,
	};

	using LoadAnimeOnlyData = YesNo<struct LoadAnimeOnlyData_tag>;

	constexpr double TicksPerSecondDefault = 30.0;

}
