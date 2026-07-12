# include "C3D_StandardSkinningDrawer.hpp"
# include "../Define/C3D_Model.hpp"

namespace crane
{

	StandardSkinningDrawer::StandardSkinningDrawer(const crane::Model* pModel, const VertexShader& vs)
		: m_pModel{ pModel }
		, m_vs{ vs }
		, m_cbBoneData{}
	{
	}

	void StandardSkinningDrawer::setConstantBuffer(const CBVSBoneData& boneData)
	{
		m_cbBoneData = boneData;
	}

	void StandardSkinningDrawer::draw(const Mat4x4& mat) const
	{
		if (m_pModel == nullptr)
		{
			return;
		}

		const auto& data = m_pModel->data;
		const auto& resource = m_pModel->resource;

		const ScopedCustomShader3D customShader{ m_vs };
		Graphics3D::SetVSConstantBuffer(4, m_cbBoneData);

		for (const auto& mesh : resource.meshes)
		{
			Graphics3D::SetVSTexture(0, mesh.boneIndices);
			Graphics3D::SetVSTexture(1, mesh.boneWeights);

			const auto& material = data.materials[mesh.materialIndex];
			if (material.material.hasDiffuseTexture)
			{
				const auto texItr = resource.diffuseTextures.find(material.texHash);
				if (texItr != resource.diffuseTextures.end())
				{
					mesh.mesh.draw(mat, texItr->second, material.material);
				}
			}
			else
			{
				mesh.mesh.draw(mat, material.material);
			}
		}
	}

}
