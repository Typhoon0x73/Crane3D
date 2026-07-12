# pragma once
# include "C3D_Define.hpp"
# include "C3D_MeshData.hpp"
# include "C3D_MaterialData.hpp"
# include "C3D_EmbeddedTextureData.hpp"
# include "C3D_BoneData.hpp"
# include "C3D_NodeData.hpp"
# include "C3D_AnimationClip.hpp"

namespace crane
{

	struct ModelData
	{
		NameHash nameHash{ 0 };
		Array<crane::MeshData> meshes{};
		Array<MaterialData> materials{};
		Array<EmbeddedTextureData> embeddedTextures{};
		Array<BoneData> bones{};
		Array<NodeData> nodes{};

		HashTable<NameHash, int32> materialIndexTable{};
		HashTable<NameHash, int32> boneIndexTable{};
		HashTable<NameHash, int32> nodeIndexTable{};

		Array<AnimationClip> animationClips{};

		// 読み込みのためのデータ。読み込み完了時にクリアする。

		HashTable<NameHash, FilePath> diffuseTexturePathTable{};

	};

}
