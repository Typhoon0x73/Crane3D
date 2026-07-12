# pragma once
# include "C3D_Define.hpp"

namespace crane
{

	struct MaterialData
	{
		NameHash nameHash{ 0 };
		NameHash texHash{ 0 };
		PhongMaterial material{};
	};

}
