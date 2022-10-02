//
// Created by leking on 22-10-2.
//

#ifndef LEKENGINE_LEK_UTILS_HPP
#define LEKENGINE_LEK_UTILS_HPP

#include <cstdio>
#include <memory>

namespace leking{
    // from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest>
    void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };
}
#endif //LEKENGINE_LEK_UTILS_HPP
