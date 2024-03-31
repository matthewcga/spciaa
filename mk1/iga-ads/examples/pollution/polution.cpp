// SPDX-FileCopyrightText: 2015 - 2021 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#include "polution.hpp"

int main() {
    ads::dim_config dim{2, 40};
    ads::timesteps_config steps{iterations, 1e-5};
    int ders = 1;

    ads::config_2d c{dim, dim, steps, ders};
    ads::problems::heat_2d sim{c};
    sim.run();
}
