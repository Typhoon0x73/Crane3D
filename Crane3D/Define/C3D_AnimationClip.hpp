# pragma once
# include "C3D_AnimationChannel.hpp"

namespace crane
{

	struct AnimationClip
	{

		double duration; ///< アニメーションクリップの長さ（Ticks単位 または 秒単位）
		double ticksPerSecond; ///< 1秒あたりのTick数（再生速度の計算に使用）
		Array<AnimationChannel> channels; ///< このモーションに含まれるすべてのノードのアニメーション情報

		inline explicit AnimationClip(
			const double d
			, const double tps
			, const uint32 channelReserveNum
		)
			: duration{ d }
			, ticksPerSecond{ tps }
			, channels{ Arg::reserve(channelReserveNum) }
		{}

	};

}
