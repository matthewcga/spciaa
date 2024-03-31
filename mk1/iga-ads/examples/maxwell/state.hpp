// SPDX-FileCopyrightText: 2015 - 2023 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef MAXWELL_STATE_HPP
#define MAXWELL_STATE_HPP

#include <array>

#include "ads/lin/tensor.hpp"

struct state {
    using field = ads::lin::tensor<double, 3>;

    field E1;
    field E2;
    field E3;

    field H1;
    field H2;
    field H3;

    explicit state(const std::array<int, 3>& shape)
    : E1{shape}
    , E2{shape}
    , E3{shape}
    , H1{shape}
    , H2{shape}
    , H3{shape} { }

    void clear() {
        zero(E1);
        zero(E2);
        zero(E3);

        zero(H1);
        zero(H2);
        zero(H3);
    }
};

#endif  // MAXWELL_STATE_HPP
