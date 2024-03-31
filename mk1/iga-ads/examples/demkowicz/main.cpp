// SPDX-FileCopyrightText: 2015 - 2023 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#include "demkowicz.hpp"

ads::bspline::basis create_basis(double a, double b, int p, int elements, int repeated_nodes,
                                 bool adapt) {
    int points = elements + 1;
    int r = repeated_nodes + 1;
    int knot_size = 2 * (p + 1) + (points - 2) * r;
    ads::bspline::knot_vector knot(knot_size);

    for (int i = 0; i <= p; ++i) {
        knot[i] = a;
        knot[knot_size - i - 1] = b;
    }

    auto x0 = 0.5;
    auto y0 = 0.99;  // 1 - 1e-5;

    for (int i = 1; i < points - 1; ++i) {
        auto t = ads::lerp(i, elements, 0.0, 1.0);

        auto s = adapt ? (t < x0 ? t / x0 * y0 : (t - x0) / (1 - x0) * (1 - y0) + y0) : t;
        for (int j = 0; j < r; ++j) {
            knot[p + 1 + (i - 1) * r + j] = ads::lerp(s, a, b);
        }
    }

    return {std::move(knot), p};
}

int main(int argc, char* argv[]) {
    if (argc != 8) {
        std::cerr
            << "Usage: demkowicz <N> <adaptations> <p_trial> <C_trial> <p_test> <C_test> <steps>"
            << std::endl;
        std::exit(1);
    }
    int n = std::atoi(argv[1]);
    bool adapt = std::atoi(argv[2]) != 0;
    int p_trial = std::atoi(argv[3]);
    int C_trial = std::atoi(argv[4]);
    int p_test = std::atoi(argv[5]);
    int C_test = std::atoi(argv[6]);
    int nsteps = std::atoi(argv[7]);

    int quad = std::max(p_trial, p_test) + 1;
    ads::dim_config trial{p_trial, n, 0.0, 1.0, quad, p_trial - 1 - C_trial};
    ads::dim_config test{p_test, n, 0.0, 1.0, quad, p_test - 1 - C_test};

    std::cout << "adaptations: " << std::boolalpha << adapt << std::endl;

    ads::timesteps_config steps{nsteps, 0.5 * 1e-2};
    int ders = 1;

    auto trial_basis_x = create_basis(0, 1, p_trial, n, p_trial - 1 - C_trial, adapt);
    // auto trial_basis_x = create_adapted_basis(0, 1, p_trial, p_trial - 1 - C_trial);

    auto dtrial_x = ads::dimension{trial_basis_x, quad, ders};

    auto trial_basis_y = ads::bspline::create_basis(0, 1, p_trial, n, p_trial - 1 - C_trial);
    auto dtrial_y = ads::dimension{trial_basis_y, quad, ders};

    auto test_basis_x = create_basis(0, 1, p_test, n, p_test - 1 - C_test, adapt);
    // auto test_basis_x = create_adapted_basis(0, 1, p_test, p_test - 1 - C_test);

    auto dtest_x = ads::dimension{test_basis_x, quad, ders};

    auto test_basis_y = ads::bspline::create_basis(0, 1, p_test, n, p_test - 1 - C_test);
    auto dtest_y = ads::dimension{test_basis_y, quad, ders};

    auto trial_dim = dtrial_x.B.dofs();
    auto test_dim = dtest_x.B.dofs();

    if (trial_dim > test_dim) {
        std::cerr << "Dimension of the trial space greater than that of test space (" << trial_dim
                  << " > " << test_dim << ")" << std::endl;
        std::exit(1);
    }

    ads::demkowicz sim{dtrial_x, dtrial_y, dtest_x, dtest_y, steps};
    sim.run();
}
