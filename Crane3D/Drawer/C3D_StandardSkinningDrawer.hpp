# pragma once
# include "C3D_IDrawer.hpp"
# include "../Define/C3D_CBVSBoneData.hpp"

namespace crane
{
	struct Model;

	class StandardSkinningDrawer : public IDrawer
	{
	public:

		explicit StandardSkinningDrawer(const crane::Model* pModel, const VertexShader& vs);

		void setConstantBuffer(const CBVSBoneData& boneData);

		virtual void draw(const Mat4x4& mat) const override;

	private:

		const crane::Model* m_pModel;
		const VertexShader& m_vs;
		ConstantBuffer<CBVSBoneData> m_cbBoneData;

	};

}
