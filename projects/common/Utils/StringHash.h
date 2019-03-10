#pragma once

#include <cstdint>
#include <cstddef>

namespace detail
{
	// FNV-1a 32bit hashing algorithm.
	constexpr uint32_t fnv1a_32(char const* s, std::size_t count)
	{
		return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
	}
}    // namespace detail

constexpr std::uint32_t operator"" _h(char const* s, std::size_t count)
{
	return detail::fnv1a_32(s, count);
}