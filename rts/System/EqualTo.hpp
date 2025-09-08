#pragma once

namespace Recoil {
	// like std::equal_to but no need to #include <functional>
	template<typename T>
	struct EqualTo
	{
		constexpr bool operator()(const T &lhs, const T &rhs) const
		{
			return lhs == rhs;
		}
	};
}

