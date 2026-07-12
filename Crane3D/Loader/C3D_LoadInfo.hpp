# pragma once
# include <Siv3D.hpp>

namespace crane
{

	struct LoadInfo
	{
		FilePath mainPath{};
		Array<FilePath> animePaths{};
		HashTable<String, FilePath> replacePathTable{};
	};

}
