# include "C3D_ModelResourceLoader.hpp"

namespace crane
{
	ModelResourceLoader::ModelResourceLoader(ModelData& data, const LoadInfo& loadInfo, ModelResource& out)
		: ResourceLoader{}
		, m_refData{ data }
		, m_loadInfo{ loadInfo }
		, m_refResource{ out }
	{
	}

	void ModelResourceLoader::loadAsync()
	{
		m_optLoadTask.reset();
		m_optLoadTask = Async(Load, std::ref(m_refData), std::ref(m_loadInfo), std::ref(m_refResource));
	}

	LoadStatus ModelResourceLoader::Load(ModelData& data, const LoadInfo& addInfo, ModelResource& out)
	{
		out.meshes.reserve(data.meshes.size());

		for (const auto& mesh : data.meshes)
		{
			const uint32 vertexNum = static_cast<uint32>(mesh.vertices.size());

			// GPUの制限を回避するため、幅を1024に固定して2Dテクスチャ化する
			constexpr uint32 TEX_WIDTH = 1024;
			const uint32 width = Min(vertexNum, TEX_WIDTH);
			const uint32 height = (vertexNum + TEX_WIDTH - 1) / TEX_WIDTH;

			Grid<Float4> bi{ width, height };
			Grid<Float4> bw{ width, height };
			for (uint32 v = 0; v < vertexNum; ++v)
			{
				const auto& ex = mesh.exVertices[v];

				const uint32 x = v % TEX_WIDTH;
				const uint32 y = v / TEX_WIDTH;
				bi[y][x] = Float4{
					static_cast<float>(Max(0, ex.boneWeights[0].index)),
					static_cast<float>(Max(0, ex.boneWeights[1].index)),
					static_cast<float>(Max(0, ex.boneWeights[2].index)),
					static_cast<float>(Max(0, ex.boneWeights[3].index))
				};
				bw[y][x] = Float4{
					ex.boneWeights[0].weight,
					ex.boneWeights[1].weight,
					ex.boneWeights[2].weight,
					ex.boneWeights[3].weight
				};
			}
			out.meshes.emplace_back(s3d::MeshData{ mesh.vertices, mesh.indices }, bi, bw, mesh.materialIndex);
		}

		for (const auto& e : data.embeddedTextures)
		{
			if (out.diffuseTextures.contains(e.keyHash))
			{
				continue;
			}
			out.diffuseTextures.emplace(e.keyHash, Texture{ std::move(MemoryReader{ e.data }), TextureDesc::MippedSRGB });
		}

		for (const auto& m : data.materials)
		{
			const NameHash texHash = m.texHash;
			if (out.diffuseTextures.contains(texHash)
				|| not data.diffuseTexturePathTable.contains(texHash))
			{
				continue;
			}

			FilePath filePath = data.diffuseTexturePathTable[texHash];
			for (auto&& [target, replace] : addInfo.replacePathTable)
			{
				if (FileSystem::FileName(filePath) == target)
				{
					filePath = replace;
					break;
				}
			}
			out.diffuseTextures.emplace(texHash, Texture{ filePath, TextureDesc::MippedSRGB });
		}

		// 不要なデータは消しておく。
		data.diffuseTexturePathTable.clear();

		return LoadStatus::Succeeded;
	}
}
