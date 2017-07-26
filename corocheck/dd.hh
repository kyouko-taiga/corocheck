#pragma once

#include <sdd/sdd.hh>

#include "model.hh"


namespace corocheck {

    struct sdd_config: public sdd::flat_set_default_configuration {
        using Identifier = std::size_t;
        using Values     = sdd::values::flat_set<std::size_t>;
    };


    using SDD = sdd::SDD<sdd_config>;
    using Hom = sdd::homomorphism<sdd_config>;


    struct containment_checker {

        const std::size_t tokens;

        sdd_config::Values operator()(const sdd_config::Values& val) const {
            auto builder = sdd::values::values_traits<sdd_config::Values>::builder();
            builder.reserve(val.size());


            // Keeps the only arcs with the correct number of tokens.
            std::copy_if(
                val.begin(),
                val.cend(),
                std::inserter(builder, builder.end()),
                [this](auto x) {
                    return (x == this->tokens);
                });

            return std::move(builder);
        }

        bool selector() const noexcept {
            return true;
        }

        bool operator==(const containment_checker& other) const noexcept {
            return this->tokens == other.tokens;
        }

    };

}


namespace std {

    template<>
    struct hash<corocheck::containment_checker> {
        std::size_t operator()(const corocheck::containment_checker& hom) {
            auto hasher = hash<std::size_t>{};
            return hasher(hom.tokens);
        }
    };

}
