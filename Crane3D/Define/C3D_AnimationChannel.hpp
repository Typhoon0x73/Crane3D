# pragma once
# include "C3D_Define.hpp"
# include "C3D_AnimationKeyDefine.hpp"

namespace crane
{

	/// @brief 1つのノード（関節など）に対する時間変化（キーフレーム群）を表すデータ
	struct AnimationChannel
	{
		NameHash nodeNameHash;
		Array<VectorKey> positionKeys;
		Array<QuaternionKey> rotationKeys;
		Array<VectorKey> scaleKeys;

		inline explicit AnimationChannel(
			const NameHash hash
			, const uint32 posReserveNum
			, const uint32 rotReserveNum
			, const uint32 scaleReserveNum
		)
			: nodeNameHash{ hash }
			, positionKeys{ Arg::reserve(posReserveNum) }
			, rotationKeys{ Arg::reserve(rotReserveNum) }
			, scaleKeys{ Arg::reserve(scaleReserveNum) }
		{}

	};

}
