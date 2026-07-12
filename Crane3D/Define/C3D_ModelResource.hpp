# pragma once
# include "C3D_Define.hpp"
# include "C3D_MeshResource.hpp"

namespace crane
{

	struct ModelResource
	{
		Array<MeshResource> meshes{};
		HashTable<NameHash, Texture> diffuseTextures{};
		HashTable<NameHash, Texture> normalTextures{};
	};

}
