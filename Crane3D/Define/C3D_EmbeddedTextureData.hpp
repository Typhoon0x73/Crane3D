# pragma once
# include "C3D_Define.hpp"

namespace crane
{

	struct EmbeddedTextureData
	{
		NameHash keyHash;
		Blob data;

		inline explicit EmbeddedTextureData(const NameHash hash, const Blob& blob)
			: keyHash{ hash }, data{ blob }
		{}

	};

}
