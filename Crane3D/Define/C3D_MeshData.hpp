# pragma once
# include "C3D_Define.hpp"
# include "C3D_VertexExData.hpp"

namespace crane
{

	struct MeshData
	{
		Array<Vertex3D> vertices;
		Array<TriangleIndex32> indices;
		Array<VertexExData> exVertices;
		uint32 materialIndex;

		inline explicit MeshData(
			const uint32 vertexReserveNum
			, const uint32 indexReserveNum
			, const uint32 materialIdx
		)
			: vertices{ Arg::reserve(vertexReserveNum) }
			, indices{ Arg::reserve(indexReserveNum) }
			, exVertices{ Arg::reserve(vertexReserveNum) }
			, materialIndex{ materialIdx }
		{}

	};

}
