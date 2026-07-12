# include "C3D_ResourceLoader.hpp"

namespace crane
{

	LoadStatus ResourceLoader::getResult()
	{
		if (not m_optLoadTask.has_value())
		{
			return LoadStatus::NonTask;
		}

		if (not m_optLoadTask->isReady())
		{
			return LoadStatus::NowLoading;
		}

		auto ret = m_optLoadTask->get();
		m_optLoadTask.reset();
		return ret;
	}

}
