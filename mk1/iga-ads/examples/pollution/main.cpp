// SPDX-FileCopyrightText: 2015 - 2023 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstdlib>

#include "pollution_2d.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: pollution <N> <p>" << std::endl;
        std::exit(1);
    }
    int n = std::atoi(argv[1]);
    int p = std::atoi(argv[2]);

    ads::dim_config dim{p, n, 0, 5000};
    ads::timesteps_config steps{600, 10};
    // timesteps_config steps{ 60000, 0.05 };

    int ders = 1;

    // config_3d c{dim, dim, dim, steps, ders};
    // pollution_3d sim{c};
    ads::config_2d c{dim, dim, steps, ders};
    ads::pollution_2d sim{c};
    sim.run();
}
