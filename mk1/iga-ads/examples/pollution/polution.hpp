// SPDX-FileCopyrightText: 2015 - 2021 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef HEAT_HEAT_2D_HPP
#define HEAT_HEAT_2D_HPP

#include <galois/Timer.h>
#include <iostream>

#include "ads/output_manager.hpp"
#include "ads/simulation.hpp"

const int iterations = 20'000;
namespace ads::problems {

class heat_2d : public simulation_2d {
private:
    using Base = simulation_2d;
    vector_type u, u_prev;

    output_manager<2> output;

public:
    explicit heat_2d(const config_2d& config)
    : Base{config}
    , u{shape()}
    , u_prev{shape()}
    , output{x.B, y.B, 200} { }

    double init_state(double /*x*/, double /*y*/) {
        return 0;
        // double dx = x - 0.5;
        // double dy = y - 0.5;
        // double r2 = std::min(8 * (dx * dx + dy * dy), 1.0);
        // return (r2 - 1) * (r2 - 1) * (r2 + 1) * (r2 + 1);
    };

private:
    const double pi = 3.14159265358979;

    void prepare_matrices() {
        y.fix_left();
        y.fix_right();
        Base::prepare_matrices();
    }

    void before() override {
        prepare_matrices();

        auto init = [this](double x, double y) { return init_state(x, y); };
        projection(u, init);
        solve(u);

        output.to_file(u, "init.data");
    }

    double s;
    void before_step(int iter, double /*t*/) override {
        using std::swap;
        swap(u, u_prev);
        const double d = 0.7;
        const double c = 10000;
        s = std::max(((cos(iter * pi / c) - d) * 1 / (1-d)), 0.);
        std::cout << "\r" << iter << "/" << iterations << " (s=" << s << ")                          \r";
    }

    void step(int iter, double /*t*/) override {
        compute_rhs(iter);
        solve(u);
    }

    void after_step(int iter, double /*t*/) override {
        if (iter % 100 == 0) {
            output.to_file(u, "out_%d.data", iter);
        }
    }

    double f(double h) {
        if (h <= 0.125) return (150 - 1200 * h) * s;
        return 0;
    }

    double e2h(double e) {
        return e / 40;
    }

    double dTy(double h) {
      if (h >= 0.8) return 0;
      return -5.2;
    }

    const int grid_size = 40; // as defined in polution.cpp
    const int cannon_x_loc = grid_size / 2;
    const int cannon_shot_time = 3'000;
    const int cannon_strength_x = 15;
    const int cannon_strength_y = 10;
    const double cone_limiter = 6.0;
    const double max_alpha = pi / cone_limiter;
    const double wave_speed = 2.0;
    const double wave_shortness = 3.0;

    double cannon(int x, int y, int iter) {
        if (iter <= cannon_shot_time)
            return 0.0;

        double i_denom = (iterations / wave_speed) - cannon_shot_time;
        // iteration denominator

        if (i_denom <= 0)
            return 0.0;

        double time = (iter - cannon_shot_time) * grid_size / i_denom;
        // time proportion where 0 is canon shot time
        // and 1 is last frame multiplayed by grid_size

        if (y > time)
            return 0.0;

        int x_prim = std::abs(cannon_x_loc - x);
        double alpha_rad = std::atan(x_prim / time);

        if (alpha_rad >= max_alpha)
            return 0.0;

        double y_prim = std::sqrt(time * time - x_prim * x_prim);

        if (y > y_prim) // if y is higher than y'
            return 0.0;

        return
            wave_shortness * (y_prim - y)
            * std::cos(alpha_rad * cone_limiter * 0.5);
    }

    const double k_x = 1.0, k_y = 0.1;
    void compute_rhs(int iter) {
        auto& rhs = u;

        zero(rhs);
        for (auto e : elements()) {
            double J = jacobian(e);
            for (auto q : quad_points()) {
                double w = weight(q);
                for (auto a : dofs_on_element(e)) {
                    value_type v = eval_basis(e, q, a);
                    value_type u = eval_fun(u_prev, e, q);

                    double b = cannon(e[0], e[1], iter);
                    double bx = (cannon(e[0] - 1, e[1], iter) - b) * cannon_strength_x;
                    double by = (cannon(e[0], e[1] - 1, iter) - b) * cannon_strength_y;
                    double gradient_prod = k_x * u.dx * v.dx + k_y * u.dy * v.dy;
                    double h = e2h(e[1]);
                    double val =
                        u.val * v.val
                        - steps.dt * gradient_prod
                        + steps.dt * dTy(h) * u.dy * v.val
                        + steps.dt * f(h) * v.val
                        - steps.dt * bx * u.dx * v.val  
                        - steps.dt * by * u.dy * v.val;
                    
                    rhs(a[0], a[1]) += val * w * J;
                }
            }
        }
    }

};

}  // namespace ads::problems

#endif  // HEAT_HEAT_2D_HPP
