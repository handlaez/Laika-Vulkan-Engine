#ifndef LE_UTILS_HPP
#define LE_UTILS_HPP

#include <random>

namespace le {

	// stolen from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	inline float randf()
	{
		thread_local static std::mt19937 rng(std::random_device{}());
		thread_local static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
		return dist(rng);
	}
}

#endif 