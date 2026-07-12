# pragma once
# include "C3D_Define.hpp"

namespace crane
{

	struct BoneData
	{
		NameHash nameHash{ 0 };
		Mat4x4 offsetMatrix{ Mat4x4::Identity() }; ///< 初期姿勢の頂点をボーンのローカル座標系へ変換する逆バインド行列
	};

}
