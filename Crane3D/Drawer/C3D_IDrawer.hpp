# pragma once
# include <Siv3D.hpp>

namespace crane
{

	class IDrawer
	{
	public:
		virtual ~IDrawer() = default;
		virtual void draw(const Mat4x4& mat) const = 0;
	};

}
