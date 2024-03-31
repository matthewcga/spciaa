// SPDX-FileCopyrightText: 2015 - 2023 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#include "stokes_split.hpp"

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: stokes <p> <n> <steps> <dt>" << std::endl;
        std::exit(-1);
    }
    int p = std::atoi(argv[1]);
    int n = std::atoi(argv[2]);
    int nsteps = std::atoi(argv[3]);
    double dt = std::atof(argv[4]);

    ads::dim_config dim{p, n};
    ads::timesteps_config steps{nsteps, dt};

    int ders = 1;

    ads::config_2d c{dim, dim, steps, ders};
    ads::problems::stokes_split sim{c};
    sim.run();
}
