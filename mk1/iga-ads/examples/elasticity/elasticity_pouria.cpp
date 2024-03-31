// SPDX-FileCopyrightText: 2015 - 2023 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#include "elasticity_pouria.hpp"

#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: elsticity_pouria <p> <n> <steps> <dt> <save_every>" << std::endl;
        std::exit(-1);
    }
    int p = std::atoi(argv[1]);
    int n = std::atoi(argv[2]);
    int nsteps = std::atoi(argv[3]);
    double dt = std::atof(argv[4]);
    int save_every = std::atoi(argv[5]);

    ads::dim_config dim{p, n};

    ads::timesteps_config steps{nsteps, dt};

    int ders = 1;

    // timesteps_config steps { 4000, 2.7e-2 };
    ads::config_2d c{dim, dim, steps, ders};
    problems::elasticity_pouria sim{c, save_every};
    sim.run();
}
