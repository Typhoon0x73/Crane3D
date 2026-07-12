# pragma once
# include <Siv3D.hpp>

namespace crane
{

	struct CBVSBoneData
	{
		std::array<Mat4x4, 256> boneMatrices;
	};

}
