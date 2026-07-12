# include "C3D_ModelDataLoader.hpp"
# include "C3D_ConvertFunc.hpp"
# include "../Define/C3D_ModelData.hpp"

namespace
{

	static int32 ProcessNode(const aiNode* pAiNode, const int32 parentIndex, crane::ModelData& outData)
	{
		if (pAiNode == nullptr)
		{
			return -1;
		}

		crane::NodeData nodeData{
			crane::ConvStringHash(pAiNode->mName)
			, parentIndex
			, pAiNode->mNumChildren
			, crane::ConvMatrix(pAiNode->mTransformation)
		};

		const int32 currentIndex = static_cast<int32>(outData.nodes.size());
		outData.nodes.push_back(nodeData);
		outData.nodeIndexTable.emplace(nodeData.nameHash, currentIndex);

		for (uint32 i = 0; i < pAiNode->mNumChildren; ++i)
		{
			const int32 childIndex = ProcessNode(pAiNode->mChildren[i], currentIndex, outData);
			outData.nodes[currentIndex].childrenIndices.emplace_back(childIndex);
		}

		return currentIndex;
	}

	static void ProcessMeshes(const aiScene* pScene, crane::ModelData& outData)
	{
		for (uint32 m = 0; m < pScene->mNumMeshes; ++m)
		{
			const aiMesh* pAiMesh = pScene->mMeshes[m];
			if (pAiMesh == nullptr)
			{
				continue;
			}
			crane::MeshData meshData{
				pAiMesh->mNumVertices
				, pAiMesh->mNumFaces
				, pAiMesh->mMaterialIndex
			};

			for (uint32 v = 0; v < pAiMesh->mNumVertices; ++v)
			{
				Vertex3D vertex{
					crane::ConvFloat3(pAiMesh->mVertices[v])
					, Float3::Zero()
					, Float2::Zero()
				};

				if (pAiMesh->HasNormals())
				{
					vertex.normal = crane::ConvFloat3(pAiMesh->mNormals[v]);
				}
				if (pAiMesh->HasTextureCoords(0))
				{
					vertex.tex = Float2{ static_cast<float>(pAiMesh->mTextureCoords[0][v].x), static_cast<float>(pAiMesh->mTextureCoords[0][v].y) };
				}
				meshData.vertices.emplace_back(vertex);

				crane::VertexExData exData;
				if (pAiMesh->HasTangentsAndBitangents())
				{
					exData.optTangent = crane::ConvFloat3(pAiMesh->mTangents[v]);
					exData.optBitangent = crane::ConvFloat3(pAiMesh->mBitangents[v]);
				}
				meshData.exVertices.emplace_back(exData);
			}

			// インデックス（面）の取得
			for (uint32 f = 0; f < pAiMesh->mNumFaces; ++f)
			{
				const aiFace& face = pAiMesh->mFaces[f];
				if (face.mNumIndices == 3)
				{
					meshData.indices.emplace_back(TriangleIndex32{ face.mIndices[0], face.mIndices[1], face.mIndices[2] });
				}
			}

			// ボーンとウェイトの取得
			if (pAiMesh->HasBones())
			{
				for (uint32 b = 0; b < pAiMesh->mNumBones; ++b)
				{
					const aiBone* pAiBone = pAiMesh->mBones[b];
					if (pAiBone == nullptr)
					{
						continue;
					}
					const crane::NameHash boneHash = crane::ConvStringHash(pAiBone->mName);

					int32 boneIndex = 0;
					const auto it = outData.boneIndexTable.find(boneHash);
					if (it == outData.boneIndexTable.end())
					{
						// 新規ボーンの登録
						boneIndex = static_cast<int32>(outData.bones.size());
						outData.bones.push_back(crane::BoneData{
							boneHash
							, crane::ConvMatrix(pAiBone->mOffsetMatrix)
						});
						outData.boneIndexTable.emplace(boneHash, boneIndex);
					}
					else
					{
						boneIndex = it->second;
					}

					// 頂点ごとのウェイトを exVertices に割り当て
					for (uint32 w = 0; w < pAiBone->mNumWeights; ++w)
					{
						const aiVertexWeight& aiWeight = pAiBone->mWeights[w];
						const uint32 vertexId = aiWeight.mVertexId;
						const float weight = aiWeight.mWeight;
						auto& exVertex = meshData.exVertices[vertexId];

						for (uint32 slot = 0; slot < exVertex.boneWeights.size(); ++slot)
						{
							auto& boneWeight = exVertex.boneWeights[slot];
							if (boneWeight.index < 0)
							{
								boneWeight.index = boneIndex;
								boneWeight.weight = weight;
								break;
							}
						}
					}
				}
			}

			outData.meshes.emplace_back(meshData);
		}

		for (uint32 t = 0; t < pScene->mNumTextures; ++t)
		{
			const auto* pTexture = pScene->mTextures[t];
			if (pTexture == nullptr)
			{
				continue;
			}
			if (pTexture->mHeight == 0)
			{
				outData.embeddedTextures.emplace_back(
					crane::ConvStringHash(pTexture->mFilename)
					, Blob{ reinterpret_cast<const void*>(pTexture->pcData), pTexture->mWidth }
				);
			}
		}
	}

	static void ProcessMaterials(const aiScene* pScene, crane::ModelData& outData)
	{
		auto getColor = [](const aiMaterial& mat, const char* key, int type, int idx, const ColorF& def)
			{
				aiColor3D color;
				if (AI_SUCCESS == mat.Get(key, type, idx, color))
				{
					return crane::ConvColorF(color);
				}
				return def;
			};

		for (uint32 i = 0; i < pScene->mNumMaterials; ++i)
		{
			const aiMaterial* pMaterial = pScene->mMaterials[i];
			if (pMaterial == nullptr)
			{
				continue;
			}

			crane::MaterialData materialData;
			aiString strValue;
			if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_NAME, strValue))
			{
				const crane::NameHash hash = crane::ConvStringHash(strValue);
				if (outData.materialIndexTable.contains(hash))
				{
					// 既に登録済み？
					continue;
				}
				materialData.nameHash = hash;
			}
			else
			{
				// 識別子がないのも困るかも。
				continue;
			}

			// 今のところSiv3DのAPIのデフォルトで送る分は１つだけ。
			if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), strValue))
			{
				const String filePath = crane::ConvString(strValue);
				materialData.texHash = filePath.hash();
				materialData.material.hasDiffuseTexture = true;
				if (not outData.diffuseTexturePathTable.contains(materialData.texHash))
				{
					outData.diffuseTexturePathTable[materialData.texHash] = filePath;
				}
			}

			// TODO: NormalMap, HightMapなどもあれば。

			materialData.material.ambientColor = getColor(*pMaterial, AI_MATKEY_COLOR_AMBIENT, ColorF{ 1.0 });
			materialData.material.diffuseColor = getColor(*pMaterial, AI_MATKEY_COLOR_DIFFUSE, ColorF{ 1.0 });
			materialData.material.specularColor = getColor(*pMaterial, AI_MATKEY_COLOR_SPECULAR, ColorF{ 0.0 });
			materialData.material.emissionColor = getColor(*pMaterial, AI_MATKEY_COLOR_EMISSIVE, ColorF{ 0.0 });

			float floatValue;
			if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_SHININESS, floatValue))
			{
				materialData.material.shininess = floatValue;
			}
			// OpacityはDiffuseのAlphaであっている？
			if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_OPACITY, floatValue))
			{
				materialData.material.diffuseColor.a = floatValue;
			}

			outData.materialIndexTable.emplace(materialData.nameHash, static_cast<int32>(outData.materials.size()));
			outData.materials.emplace_back(materialData);
		}
	}

	static void ProcessAnimations(const aiScene* pScene, crane::ModelData& outData)
	{
		for (uint32 i = 0; i < pScene->mNumAnimations; ++i)
		{
			const aiAnimation* pAiAnim = pScene->mAnimations[i];
			if (pAiAnim == nullptr)
			{
				continue;
			}
			crane::AnimationClip clip{
				pAiAnim->mDuration
				, ((pAiAnim->mTicksPerSecond != 0.0) ? pAiAnim->mTicksPerSecond : crane::TicksPerSecondDefault)
				, pAiAnim->mNumChannels
			};

			for (uint32 c = 0; c < pAiAnim->mNumChannels; ++c)
			{
				const aiNodeAnim* pAiChannel = pAiAnim->mChannels[c];
				if (pAiChannel == nullptr)
				{
					continue;
				}
				crane::AnimationChannel channel{
					crane::ConvStringHash(pAiChannel->mNodeName)
					, pAiChannel->mNumPositionKeys
					, pAiChannel->mNumRotationKeys
					, pAiChannel->mNumScalingKeys
				};

				for (uint32 pk = 0; pk < pAiChannel->mNumPositionKeys; ++pk)
				{
					channel.positionKeys.emplace_back(
						crane::VectorKey{
							pAiChannel->mPositionKeys[pk].mTime
							, crane::ConvFloat3(pAiChannel->mPositionKeys[pk].mValue)
						}
					);
				}

				for (uint32 rk = 0; rk < pAiChannel->mNumRotationKeys; ++rk)
				{
					channel.rotationKeys.emplace_back(
						crane::QuaternionKey{
							pAiChannel->mRotationKeys[rk].mTime
							, crane::ConvQuaternion(pAiChannel->mRotationKeys[rk].mValue)
						}
					);
				}

				for (uint32 sk = 0; sk < pAiChannel->mNumScalingKeys; ++sk)
				{
					channel.scaleKeys.emplace_back(
						crane::VectorKey{
							pAiChannel->mScalingKeys[sk].mTime
							, crane::ConvFloat3(pAiChannel->mScalingKeys[sk].mValue)
						}
					);
				}

				clip.channels.emplace_back(channel);
			}
			outData.animationClips.push_back(clip);
		}
	}

}

namespace crane
{

	ModelDataLoader::ModelDataLoader(const FilePath& path, ModelData& out, const LoadAnimeOnlyData animeOnly)
		: ResourceLoader{}
		, m_filePath{ path }
		, m_refModelData{ out }
		, m_animeOnly{ animeOnly }
	{
	}

	void ModelDataLoader::loadAsync()
	{
		m_optLoadTask.reset();
		m_optLoadTask = Async(Load, m_filePath, std::ref(m_refModelData), m_animeOnly);
	}

	LoadStatus ModelDataLoader::Load(const FilePath& path, ModelData& out, const LoadAnimeOnlyData animeOnly)
	{
		Assimp::Importer importer{};

		// ノード名の自動生成？切らないとアニメと一致しなくなる。
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

		const aiScene* pScene = importer.ReadFile(path.toUTF8(), LoadFlagDefault);

		if (not pScene
			|| (not animeOnly && (pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
			|| not pScene->mRootNode)
		{
			Logger << U"Assimp Load Error: " << Unicode::FromUTF8(importer.GetErrorString());
			return LoadStatus::Failed;
		}

		# if SIV3D_BUILD(DEBUG) && defined(CRANE3D_USE_DUMP_ASSIMP_DATA)
		Dump::AssimpScene(pScene, FileSystem::BaseName(path) + U"_AssimpData.jsonc");
		# endif //SIV3D_BUILD(DEBUG) && defined(CRANE3D_USE_DUMP_ASSIMP_DATA)

		out.meshes.reserve(pScene->mNumMeshes);
		out.materials.reserve(pScene->mNumMaterials);
		out.embeddedTextures.reserve(pScene->mNumTextures);

		out.nameHash = ConvStringHash(pScene->mName);

		if (not animeOnly)
		{
			ProcessNode(pScene->mRootNode, -1, out);
			ProcessMeshes(pScene, out);
			ProcessMaterials(pScene, out);
		}

		ProcessAnimations(pScene, out);

		# if SIV3D_BUILD(DEBUG) && defined(CRANE3D_USE_DUMP_MODEL_DATA)
		Dump::ModelData(out, FileSystem::BaseName(path) + U"_ModelData.jsonc");
		# endif //SIV3D_BUILD(DEBUG) && defined(CRANE3D_USE_DUMP_MODEL_DATA)

		return LoadStatus::Succeeded;
	}
}
