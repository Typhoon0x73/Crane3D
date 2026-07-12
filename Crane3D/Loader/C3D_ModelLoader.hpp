# pragma once
# include "C3D_ResourceLoader.hpp"
# include "C3D_LoadInfo.hpp"
# include "../Define/C3D_Model.hpp"

namespace crane
{

	class ModelLoader : public ResourceLoader
	{
	public:

		explicit ModelLoader(const LoadInfo& info, crane::Model& out);

		virtual void loadAsync() override;

		static LoadStatus Load(const LoadInfo& info, crane::Model& out);

	private:

		LoadInfo m_loadInfo;
		crane::Model& m_refModel;

	};
}
