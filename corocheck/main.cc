#include <array>
#include <bitset>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/array.hpp>

#include "corocheck.hh"


// using marking_type    = std::bitset<2>;
using marking_type    = std::array<std::size_t, 2>;
using transition_type = petrinet::transition<2, marking_type>;


int main() {

    auto transitions = {
        transition_type({1, 0}, {0, 1}),
        transition_type({0, 1}, {1, 0})
    };

    // auto initial_marking = marking_type(1);
    auto initial_marking = marking_type({15, 0});
    auto seen            = std::unordered_set<marking_type>({initial_marking});
    auto to_process      = std::vector<marking_type>({initial_marking});

    boost::archive::text_oarchive archive(std::cout);
    archive << initial_marking;
    std::cout << std::endl;

    auto begin = std::chrono::high_resolution_clock::now();

    while (!to_process.empty()) {
        auto marking = to_process.back();
        to_process.pop_back();
        marking_type successors[transitions.size()];

        std::size_t i = 0;
        for (auto t: transitions) {
            if (t.fire(marking, successors[i])) {
                if (seen.find(successors[i]) == seen.end()) {
                    to_process.push_back(successors[i]);
                    seen.insert(successors[i]);
                }
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << seen.size() << " state(s)" << std::endl
    << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
    << "ms" << std::endl;

    return 0;
}
