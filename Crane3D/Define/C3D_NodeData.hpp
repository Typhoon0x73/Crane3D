# pragma once
# include "C3D_Define.hpp"

namespace crane
{
	/// @brief モデルの階層構造（親子関係）を構成するノードを表すデータ
	struct NodeData
	{
		NameHash nameHash;
		int32 parentIndex;
		Array<int32> childrenIndices;
		Mat4x4 localTransform;

		inline explicit NodeData(
			const NameHash hash
			, const int32 parentIdx
			, const uint32 childReserveNum
			, const Mat4x4& trans
		)
			: nameHash{ hash }
			, parentIndex{ parentIdx }
			, childrenIndices{ Arg::reserve(childReserveNum) }
			, localTransform{ trans }
		{}

	};

}
