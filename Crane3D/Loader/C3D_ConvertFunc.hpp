# pragma once
# include <Siv3D.hpp>
# include "../Define/AssimpWrapper.hpp"

namespace crane
{

	inline static String ConvString(const aiString& from)
	{
		return Unicode::FromUTF8(std::string_view(from.C_Str(), from.length));
	}

	inline static NameHash ConvStringHash(const aiString& from)
	{
		return ConvString(from).hash();
	}

	inline static Float3 ConvFloat3(const aiVector3D& from)
	{
		return Float3
		{
			static_cast<float>(from.x)
			, static_cast<float>(from.y)
			, static_cast<float>(from.z)
		};
	}

	inline static Quaternion ConvQuaternion(const aiQuaternion& from)
	{
		return Quaternion
		{
			static_cast<float>(from.x)
			, static_cast<float>(from.y)
			, static_cast<float>(from.z)
			, static_cast<float>(from.w)
		};
	}

	inline static Mat4x4 ConvMatrix(const aiMatrix4x4& from)
	{
		return Mat4x4
		{
			static_cast<float>(from.a1), static_cast<float>(from.b1), static_cast<float>(from.c1), static_cast<float>(from.d1),
			static_cast<float>(from.a2), static_cast<float>(from.b2), static_cast<float>(from.c2), static_cast<float>(from.d2),
			static_cast<float>(from.a3), static_cast<float>(from.b3), static_cast<float>(from.c3), static_cast<float>(from.d3),
			static_cast<float>(from.a4), static_cast<float>(from.b4), static_cast<float>(from.c4), static_cast<float>(from.d4)
		};
	}

	inline static ColorF ConvColorF(const aiColor3D& from)
	{
		return ColorF
		{
			static_cast<double>(from.r)
			, static_cast<double>(from.g)
			, static_cast<double>(from.b)
		};
	}

	inline static ColorF ConvColorF(const aiColor4D& from)
	{
		return ColorF
		{
			static_cast<double>(from.r)
			, static_cast<double>(from.g)
			, static_cast<double>(from.b)
			, static_cast<double>(from.a)
		};
	}

}
