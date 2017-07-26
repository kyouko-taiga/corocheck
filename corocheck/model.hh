#pragma once

#include <bitset>

#include "corocheck.hh"


namespace corocheck {

    constexpr auto place_count = 2;

    using marking_type    = std::bitset<place_count>;
    using transition_type = transition<place_count, marking_type>;

    auto transitions = {
        transition_type({1, 0}, {0, 1}),
        transition_type({0, 1}, {1, 0}),
    };

    auto initial_marking = marking_type("01");

}
