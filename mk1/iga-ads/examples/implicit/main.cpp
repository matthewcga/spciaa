// SPDX-FileCopyrightText: 2015 - 2023 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstdlib>

#include "implicit.hpp"

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: implicit <p> <n> <steps> <dt> <save_every>" << std::endl;
        std::exit(-1);
    }
    int p = std::atoi(argv[1]);
    int n = std::atoi(argv[2]);
    int nsteps = std::atoi(argv[3]);
    double dt = std::atof(argv[4]);
    int save_every = std::atoi(argv[5]);

    // dim_config dim{ 2, 80 };
    ads::dim_config dim{p, n};

    // timesteps_config steps{ 100, 1e-2 };
    ads::timesteps_config steps{nsteps, dt};

    int ders = 1;

    ads::config_2d c{dim, dim, steps, ders};
    ads::implicit_2d sim{c, save_every};
    sim.run();
}
