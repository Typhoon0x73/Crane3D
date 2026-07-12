# pragma once
# include <Siv3D.hpp>

namespace crane
{

	/// @brief 位置またはスケールのキーフレームを表すデータ
	struct VectorKey
	{
		double time{ 0.0 };
		Float3 value{ 0.0f, 0.0f, 0.0f };
	};

	/// @brief 回転のキーフレームを表すデータ
	struct QuaternionKey
	{
		double time{ 0.0 };
		Quaternion value{ 0.0f, 0.0f, 0.0f, 1.0f };
	};

}
