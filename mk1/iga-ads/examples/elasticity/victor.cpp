// SPDX-FileCopyrightText: 2015 - 2023 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstdlib>

#include "elasticity_victor.hpp"

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: elsticity_victor <p> <n> <steps> <dt> <save_every>" << std::endl;
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
    ads::config_3d c{dim, dim, dim, steps, ders};
    problems::elasticity_victor sim{c, save_every};
    sim.run();
}
