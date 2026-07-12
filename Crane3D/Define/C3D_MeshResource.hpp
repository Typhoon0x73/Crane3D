# pragma once
# include <Siv3D.hpp>

namespace crane
{

	struct MeshResource
	{
		Mesh mesh;
		RenderTexture boneIndices;
		RenderTexture boneWeights;
		int32 materialIndex;

		inline explicit MeshResource(const s3d::MeshData& md, const Grid<Float4>& bi, const Grid<Float4>& bw, const int32 matIdx)
			: mesh{ md }, boneIndices{ bi }, boneWeights{ bw }, materialIndex{ matIdx }
		{}

	};

}
