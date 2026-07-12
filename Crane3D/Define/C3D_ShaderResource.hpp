# pragma once
# include "C3D_Define.hpp"

namespace crane
{

	struct ShaderResource
	{
		HashTable<NameHash, VertexShader> vsTable{};
		HashTable<NameHash, PixelShader> psTable{};
	};

}
