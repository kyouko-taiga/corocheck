#pragma once

#include <array>


namespace corocheck {

    template<std::size_t place_count, typename M>
    struct transition {

        using arc_type = std::array<std::size_t, place_count>;

        arc_type pre;
        arc_type post;

        transition(const arc_type& pre, const arc_type& post)
            : pre(pre), post(post) {}
        transition(const transition<place_count, M>& other) = default;
        transition(transition<place_count, M>&& other)      = default;

        bool fire(const M& marking, M& result) const {
            for (std::size_t i = 0; i < place_count; ++i) {
                if (marking[i] < pre[i]) {
                    return false;
                }
                result[i] = marking[i] - pre[i] + post[i];
            }
            return true;
        }

    };

}

namespace std {

    template<typename T, size_t size>
    struct hash<array<T, size>> {
        std::size_t operator()(const array<T, size>& marking) {
            size_t result = 17;
            auto   hasher = hash<T>{};
            for (auto x: marking) {
                result ^= hasher(x) + 0x9e3779b9 + (result << 6) + (result >> 2);
            }
            return result;
        }
    };

}
