# pragma once
# include <Siv3D.hpp>

namespace crane
{
	/// @brief 頂点に対する1つのボーンの影響を表すデータ
	struct BoneWeight
	{
		int32 index{ -1 };		///< 影響を与えるボーンのインデックス
		float weight{ 0.0 };	///< そのボーンからの影響度 (0.0 から 1.0)
	};

}
