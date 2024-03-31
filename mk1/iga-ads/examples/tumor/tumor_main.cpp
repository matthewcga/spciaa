// SPDX-FileCopyrightText: 2015 - 2023 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#include "ads/simulation.hpp"
#include "tumor.hpp"
#include "vasculature.hpp"

// clang-tidy 12 complains about deleted default constructor not
// initializing some members
//
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct sim_params {
    int p;                        // 2
    int elems;                    // 80
    ads::timesteps_config steps;  // 10000, 0.1
    int plot_every;
    tumor::params tumor_params;
    tumor::vasc::config vasc_config;
};

sim_params parse_params(char* args[], int idx) {
    using std::atof;
    using std::atoi;
    auto next = [&]() { return args[idx++]; };
    auto next_int = [&]() { return atoi(next()); };
    auto next_float = [&]() { return atof(next()); };

    int p = next_int();
    int elems = next_int();
    int nsteps = next_int();
    double dt = next_float();
    int plot_every = next_int();

    // tumor parameters
    tumor::params tumor;

    tumor.tau_b = next_float();

    tumor.o_prol_TC = next_float();
    tumor.o_death_TC = next_float();
    tumor.t_prol_TC = next_float();
    tumor.t_death_TC = next_float();

    tumor.P_b = next_float();
    tumor.r_b = next_float();

    tumor.beta_m = next_float();
    tumor.gamma_a = next_float();
    tumor.chi_aA = next_float();
    tumor.gamma_oA = next_float();

    tumor.diff_c = next_float();
    tumor.cons_c = next_float();

    // 3D only
    tumor.alpha_0 = next_float();
    tumor.gamma_T = next_float();
    tumor.alpha_1 = next_float();

    // Vasculature parameters
    tumor::vasc::config vasc;

    vasc.init_stability = next_float();
    vasc.degeneration = next_float();
    vasc.t_ec_sprout = next_float();
    vasc.segment_length = next_float();
    vasc.t_ec_collapse = next_float();
    vasc.c_min = next_float();

    // 2D only
    vasc.t_ec_migr = next_float();

    // 3D only
    vasc.r_sprout = next_float();
    vasc.r_max = next_float();
    vasc.t_ec_switch = next_float();
    vasc.c_switch = next_float();
    vasc.dilatation = next_float();

    return {p, elems, {nsteps, dt}, plot_every, tumor, vasc};
}

int main(int /*argc*/, char* argv[]) {
    sim_params sp = parse_params(argv, 1);

    ads::dim_config dim{sp.p, sp.elems, 0, 3000.0};
    int ders = 1;

    ads::config_2d c{dim, dim, sp.steps, ders};

    tumor::vasc::random_vasculature rand_vasc{sp.vasc_config, 0};
    tumor::tumor_2d sim{c, sp.tumor_params, sp.plot_every, rand_vasc()};
    sim.run();
}
