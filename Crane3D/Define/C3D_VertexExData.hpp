# pragma once
# include <Siv3D.hpp>
# include "C3D_BoneWeight.hpp"

namespace crane
{
	/// @brief Vertex3Dと1:1の関係になる追加頂点データ
	struct VertexExData
	{
		Optional<Float3> optTangent{ none };
		Optional<Float3> optBitangent{ none };
		std::array<BoneWeight, 4> boneWeights{};
	};

}
