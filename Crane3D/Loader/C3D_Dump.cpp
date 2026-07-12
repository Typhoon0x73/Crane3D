# include "C3D_Dump.hpp"
# include "C3D_ConvertFunc.hpp"

# if SIV3D_BUILD(DEBUG)

constexpr const char32_t* TAB_STR = U"\t";
constexpr size_t TAB_STR_LEN = 1;
# define WRITE_BEGIN_BRACKET(name)\
			writer << tab << U"\"" << name << U"\"" << U":{";\
			tab += TAB_STR;
# define WRITE_DATA(name, data)\
			writer << tab << U"\"" << name << U"\"" << U":" << data << U",";
# define WRITE_ARRAY(name, data)\
			writer << tab << U"\"" << name << U"\"" << U":" << data.replace(U"{", U"[").replace(U"}", U"]") << U",";
# define WRITE_STR(name, data)\
			writer << tab << U"\"" << name << U"\"" << U":" << U"\"" << data << U"\"" << U",";
# define WRITE_END_BRACKET\
			tab.pop_back_N(TAB_STR_LEN);\
			writer << tab << U"},";

namespace
{

	static void AiNode(TextWriter& writer, String tab, const aiNode* pNode)
	{
		if (pNode == nullptr)
		{
			return;
		}

		WRITE_STR(U"Name", crane::ConvString(pNode->mName));
		WRITE_DATA(U"NameHash", crane::ConvStringHash(pNode->mName));
		WRITE_STR(U"Transformation", Format(crane::ConvMatrix(pNode->mTransformation)));
		WRITE_DATA(U"NumChildren", pNode->mNumChildren);

		WRITE_BEGIN_BRACKET(U"Children");
		for (uint32 i = 0; i < pNode->mNumChildren; ++i)
		{
			WRITE_BEGIN_BRACKET(Format(i));
			AiNode(writer, tab, pNode->mChildren[i]);
			WRITE_END_BRACKET;
		}
		WRITE_END_BRACKET;
	}

}

namespace crane
{

	void Dump::AssimpScene(const aiScene* pScene, const FilePathView path)
	{
		if (pScene == nullptr)
		{
			return;
		}

		TextWriter writer{ path };
		if (not writer.isOpen())
		{
			return;
		}

		writer << U"{";
		String tab = TAB_STR;
		WRITE_BEGIN_BRACKET(U"RootNode");
		AiNode(writer, tab, pScene->mRootNode);
		WRITE_END_BRACKET;
		WRITE_DATA(U"NumMeshes", pScene->mNumMeshes);
		WRITE_BEGIN_BRACKET(U"meshes");
		for (uint32 m = 0; m < pScene->mNumMeshes; ++m)
		{
			WRITE_BEGIN_BRACKET(Format(m));
			const aiMesh* pMesh = pScene->mMeshes[m];
			if (pMesh != nullptr)
			{
				WRITE_STR(U"Name", ConvString(pMesh->mName));
				WRITE_DATA(U"NameHash", ConvStringHash(pMesh->mName));
				WRITE_DATA(U"NumVertices", pMesh->mNumVertices);
				WRITE_DATA(U"NumFaces", pMesh->mNumFaces);
				WRITE_DATA(U"NumBones", pMesh->mNumBones);
				WRITE_BEGIN_BRACKET(U"vertices");
				for (uint32 v = 0; v < pMesh->mNumVertices; ++v)
				{
					WRITE_BEGIN_BRACKET(Format(v));
					WRITE_DATA(U"NumUVChannels", pMesh->GetNumUVChannels());
					WRITE_DATA(U"NumColorChannels", pMesh->GetNumColorChannels());
					WRITE_STR(U"Position", Format(ConvFloat3(pMesh->mVertices[v])));
					WRITE_STR(U"Normal", Format(ConvFloat3(pMesh->mNormals[v])));
					if (pMesh->HasTextureCoords(0))
					{
						WRITE_STR(U"UV", Format(ConvFloat3(pMesh->mTextureCoords[0][v])));
					}
					WRITE_END_BRACKET;
				}
				WRITE_END_BRACKET;
			}
			WRITE_END_BRACKET;

			WRITE_BEGIN_BRACKET(U"indices");
			for (uint32 f = 0; f < pMesh->mNumFaces; ++f)
			{
				WRITE_BEGIN_BRACKET(Format(f));
				WRITE_DATA(U"NumIndices", pMesh->mFaces[f].mNumIndices);
				WRITE_ARRAY(U"indices", Format(Array<uint32>(pMesh->mFaces[f].mIndices, pMesh->mFaces[f].mIndices + pMesh->mFaces[f].mNumIndices)));
				WRITE_END_BRACKET;
			}
			WRITE_END_BRACKET;

			WRITE_BEGIN_BRACKET(U"bones");
			for (uint32 b = 0; b < pMesh->mNumBones; ++b)
			{
				WRITE_BEGIN_BRACKET(Format(b));
				const aiBone* pBone = pMesh->mBones[b];
				if (pBone != nullptr)
				{
					WRITE_STR(U"Name", ConvString(pBone->mName));
					WRITE_DATA(U"NameHash", ConvStringHash(pBone->mName));
					WRITE_STR(U"OffsetMatrix", Format(ConvMatrix(pBone->mOffsetMatrix)));
					WRITE_DATA(U"NumWeights", pBone->mNumWeights);

					WRITE_BEGIN_BRACKET(U"weights");
					for (uint32 w = 0; w < pBone->mNumWeights; ++w)
					{
						WRITE_BEGIN_BRACKET(Format(w));
						WRITE_DATA(U"VertexID", pBone->mWeights[w].mVertexId);
						WRITE_DATA(U"Weight", pBone->mWeights[w].mWeight);
						WRITE_END_BRACKET;
					}
					WRITE_END_BRACKET;
				}
				WRITE_END_BRACKET;
			}
			WRITE_END_BRACKET;
		}
		WRITE_END_BRACKET;

		WRITE_BEGIN_BRACKET(U"Animations");
		for (uint32 a = 0; a < pScene->mNumAnimations; ++a)
		{
			WRITE_BEGIN_BRACKET(Format(a));
			const aiAnimation* pAnim = pScene->mAnimations[a];
			if (pAnim != nullptr)
			{
				WRITE_STR(U"Name", ConvString(pAnim->mName));
				WRITE_DATA(U"NameHash", ConvStringHash(pAnim->mName));
				WRITE_DATA(U"Duration", pAnim->mDuration);
				WRITE_DATA(U"TicksPerSecond", pAnim->mTicksPerSecond);
				WRITE_DATA(U"NumChannels", pAnim->mNumChannels);

				WRITE_BEGIN_BRACKET(U"Channels");
				for (uint32 c = 0; c < pAnim->mNumChannels; ++c)
				{
					WRITE_BEGIN_BRACKET(Format(c));
					const aiNodeAnim* pChannel = pAnim->mChannels[c];
					if (pChannel != nullptr)
					{
						WRITE_STR(U"NodeName", ConvString(pChannel->mNodeName));
						WRITE_DATA(U"NodeNameHash", ConvStringHash(pChannel->mNodeName));
						WRITE_DATA(U"NumPositionKeys", pChannel->mNumPositionKeys);
						WRITE_DATA(U"NumRotationKeys", pChannel->mNumRotationKeys);
						WRITE_DATA(U"NumScalingKeys", pChannel->mNumScalingKeys);
						WRITE_BEGIN_BRACKET(U"PositionKeys");
						for (uint32 k = 0; k < pChannel->mNumPositionKeys; ++k)
						{
							WRITE_BEGIN_BRACKET(Format(k));
							WRITE_DATA(U"Time", pChannel->mPositionKeys[k].mTime);
							WRITE_STR(U"Value", Format(ConvFloat3(pChannel->mPositionKeys[k].mValue)));
							WRITE_END_BRACKET;
						}
						WRITE_END_BRACKET;

						WRITE_BEGIN_BRACKET(U"RotationKeys");
						for (uint32 k = 0; k < pChannel->mNumRotationKeys; ++k)
						{
							WRITE_BEGIN_BRACKET(Format(k));
							WRITE_DATA(U"Time", pChannel->mRotationKeys[k].mTime);
							WRITE_STR(U"Value", Format(ConvQuaternion(pChannel->mRotationKeys[k].mValue)));
							WRITE_END_BRACKET;
						}
						WRITE_END_BRACKET;

						WRITE_BEGIN_BRACKET(U"ScaleKeys");
						for (uint32 k = 0; k < pChannel->mNumScalingKeys; ++k)
						{
							WRITE_BEGIN_BRACKET(Format(k));
							WRITE_DATA(U"Time", pChannel->mScalingKeys[k].mTime);
							WRITE_STR(U"Value", Format(ConvFloat3(pChannel->mScalingKeys[k].mValue)));
							WRITE_END_BRACKET;
						}
						WRITE_END_BRACKET;
					}
					WRITE_END_BRACKET;
				}
				WRITE_END_BRACKET;
			}
			WRITE_END_BRACKET;
		}
		WRITE_END_BRACKET;
		writer << U"}";
	}

	void Dump::ModelData(const crane::ModelData& data, const FilePathView path)
	{
		TextWriter writer{ path };
		if (not writer.isOpen())
		{
			return;
		}

		String tab = TAB_STR;
		writer << U"{";
		WRITE_DATA(U"NumNodes", data.nodes.size());
		WRITE_BEGIN_BRACKET(U"Nodes");
		for (int32 i = 0; const auto& n : data.nodes)
		{
			WRITE_BEGIN_BRACKET(Format(i++));
			WRITE_DATA(U"NameHash", n.nameHash);
			WRITE_DATA(U"ParentIndex", n.parentIndex);
			WRITE_STR(U"LocalTransform", Format(n.localTransform));
			WRITE_ARRAY(U"ChildrenIndices", Format(n.childrenIndices));
			WRITE_END_BRACKET;
		}
		WRITE_END_BRACKET;

		WRITE_DATA(U"NumMeshes", data.meshes.size());

		WRITE_BEGIN_BRACKET(U"Meshes");
		for (uint32 i = 0; const auto& m : data.meshes)
		{
			WRITE_BEGIN_BRACKET(Format(i++));
			WRITE_DATA(U"NumVertices", m.vertices.size());
			WRITE_DATA(U"NumIndices", m.indices.size());
			WRITE_DATA(U"MaterialIndex", m.materialIndex);

			WRITE_BEGIN_BRACKET(U"Vertices");
			for (uint32 k = 0; const auto& v : m.vertices)
			{
				WRITE_BEGIN_BRACKET(Format(k++));
				WRITE_STR(U"Position", Format(v.pos));
				WRITE_STR(U"Normal", Format(v.normal));
				WRITE_STR(U"Tex", Format(v.tex));
				WRITE_END_BRACKET;
			}
			WRITE_END_BRACKET;

			WRITE_BEGIN_BRACKET(U"Indices");
			for (uint32 k = 0; const auto& idx : m.indices)
			{
				WRITE_BEGIN_BRACKET(Format(k++));
				WRITE_DATA(U"i0", idx.i0);
				WRITE_DATA(U"i1", idx.i1);
				WRITE_DATA(U"i2", idx.i2);
				WRITE_END_BRACKET;
			}
			WRITE_END_BRACKET;

			WRITE_BEGIN_BRACKET(U"exVertices");
			for (uint32 k = 0; const auto& v : m.exVertices)
			{
				WRITE_BEGIN_BRACKET(Format(k++));
				if (v.optTangent)
				{
					WRITE_STR(U"Tangent", Format(*v.optTangent));
				}
				if (v.optBitangent)
				{
					WRITE_STR(U"Bitangent", Format(*v.optBitangent));
				}
				WRITE_BEGIN_BRACKET(U"boneWeights");
				for (uint32 bwi = 0; const auto& bw : v.boneWeights)
				{
					WRITE_BEGIN_BRACKET(Format(bwi++));
					WRITE_DATA(U"Index", bw.index);
					WRITE_DATA(U"Weight", bw.weight);
					WRITE_END_BRACKET;
				}
				WRITE_END_BRACKET;
				WRITE_END_BRACKET;
			}
			WRITE_END_BRACKET;
		}
		WRITE_END_BRACKET;

		WRITE_BEGIN_BRACKET(U"exVertices");
		for (uint32 i = 0; const auto& anim : data.animationClips)
		{
			WRITE_BEGIN_BRACKET(Format(i++));
			WRITE_BEGIN_BRACKET(U"exVertices");
			for (uint32 k = 0; const auto& channel : anim.channels)
			{
				WRITE_BEGIN_BRACKET(Format(k++));
				WRITE_DATA(U"hash", channel.nodeNameHash);
				WRITE_BEGIN_BRACKET(U"posKeys");
				for (uint32 ki = 0; const auto& key : channel.positionKeys)
				{
					WRITE_BEGIN_BRACKET(Format(ki++));
					WRITE_DATA(U"time", key.time);
					WRITE_DATA(U"value", Format(key.value));
					WRITE_END_BRACKET;
				}
				WRITE_END_BRACKET;
				WRITE_BEGIN_BRACKET(U"rotKeys");
				for (uint32 ki = 0; const auto& key : channel.rotationKeys)
				{
					WRITE_BEGIN_BRACKET(Format(ki++));
					WRITE_DATA(U"time", key.time);
					WRITE_DATA(U"value", Format(key.value));
					WRITE_END_BRACKET;
				}
				WRITE_END_BRACKET;
				WRITE_BEGIN_BRACKET(U"scaleKeys");
				for (uint32 ki = 0; const auto& key : channel.scaleKeys)
				{
					WRITE_BEGIN_BRACKET(Format(ki++));
					WRITE_DATA(U"time", key.time);
					WRITE_DATA(U"value", Format(key.value));
					WRITE_END_BRACKET;
				}
				WRITE_END_BRACKET;
				WRITE_END_BRACKET;
			}
			WRITE_END_BRACKET;

			WRITE_DATA(U"duration", anim.duration);
			WRITE_DATA(U"tps", anim.ticksPerSecond);
			WRITE_END_BRACKET;
		}
		WRITE_END_BRACKET;

		WRITE_BEGIN_BRACKET(U"nodeIndexTable");
		for (uint32 i = 0; auto&& [hash, idx] : data.nodeIndexTable)
		{
			WRITE_BEGIN_BRACKET(Format(i++));
			WRITE_DATA(U"hash", hash);
			WRITE_DATA(U"idx", idx);
			WRITE_END_BRACKET;
		}
		WRITE_END_BRACKET;

		WRITE_BEGIN_BRACKET(U"boneIndexTable");
		for (uint32 i = 0; auto&& [hash, idx] : data.boneIndexTable)
		{
			WRITE_BEGIN_BRACKET(Format(i++));
			WRITE_DATA(U"hash", hash);
			WRITE_DATA(U"idx", idx);
			WRITE_END_BRACKET;
		}
		WRITE_END_BRACKET;

		writer << U"}";
	}

}

#endif // SIV3D_BUILD(DEBUG)
