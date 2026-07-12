# pragma once
# include "C3D_LoaderDefine.hpp"

namespace crane
{

	class ResourceLoader
	{
	public:

		ResourceLoader() = default;
		virtual ~ResourceLoader() = default;

		virtual void loadAsync() = 0;

		virtual LoadStatus getResult();

	protected:

		Optional<AsyncTask<LoadStatus>> m_optLoadTask{ none };
	};

}
