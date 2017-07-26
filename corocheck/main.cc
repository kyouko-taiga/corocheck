#include <chrono>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <set>

#include <sdd/sdd.hh>

//#include <boost/archive/text_oarchive.hpp>
//#include <boost/serialization/array.hpp>

#include "corocheck.hh"
#include "dd.hh"


int main() {
    using namespace corocheck;

    // Initialize sddlib.
    auto manager = sdd::init<sdd_config>();

    // Create a variable ordering.
    sdd::order_builder<sdd_config> ob;
    for (std::size_t i = 0; i < place_count; ++i) {
        ob.push(i);
    }
    sdd::order<sdd_config> order(ob);

    // Create the DD of seen states.
    SDD markings(order, [](sdd_config::Identifier place) {
        return sdd_config::Values{ initial_marking[place] };
    });

    std::set<Hom> homset;
    for (std::size_t i = 0; i < place_count; ++i) {
//        homset.insert(sdd::function<sdd_config>(
//            order, i, containment_checker{ initial_marking[i] }));
        homset.insert(sdd::function<sdd_config>(
            order, i, containment_checker{ 0 }));
    }
    auto filter = sdd::intersection(order, homset.begin(), homset.end());
    filter      = sdd::rewrite(order, filter);

    auto res = filter(order, markings);
    std::cout << res.empty() << std::endl;

    return 0;

    auto seen       = std::unordered_set<marking_type>({initial_marking});
    auto to_process = std::vector<marking_type>({initial_marking});

//    boost::archive::text_oarchive archive(std::cout);
//    archive << initial_marking;
//    std::cout << std::endl;

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
