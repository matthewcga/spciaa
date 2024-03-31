// SPDX-FileCopyrightText: 2015 - 2021 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef HEAT_HEAT_2D_HPP
#define HEAT_HEAT_2D_HPP

#include <galois/Timer.h>
#include <iostream>

#include "ads/output_manager.hpp"
#include "ads/simulation.hpp"

const int iterations = 30000;
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

    const int max_iter = 30'000;
    const int cannon_shot_time = 7'000;
    const double cannon_strength = 0.1;
    const double cone_limiter = 2.0;
    const double max_alpha = pi / 4;
    // the bigger the value the smaller the cone

    double cannon(double x, double y, int iter) {
        if (iter - cannon_shot_time)
            return 0.0;

        double time = (iter - cannon_shot_time)
            / (max_iter - cannon_shot_time);
        double alpha_rad = std::atan(x / time);

        if (alpha_rad >= max_alpha)
            return 0.0;

        double y_prim = x / std::tan(alpha_rad);

        if (y > y_prim) // if y is higher than 'y
            return 0.0;

        return (1.0 - (y_prim - y))
             * sin(alpha_rad * 2.0)
             * cannon_strength;
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

                    double gradient_prod = std::max(
                        k_x * u.dx * v.dx
                        + k_y * u.dy * v.dy
                        - cannon(e[0], e[1], iter), 0.0);
                    double h = e2h(e[1]);
                    double val =
                        u.val * v.val
                        - steps.dt * gradient_prod
                        + steps.dt * dTy(h) * u.dy * v.val
                        + steps.dt * f(h) * v.val;
                    
                    rhs(a[0], a[1]) += val * w * J;
                }
            }
        }
    }

};

}  // namespace ads::problems

#endif  // HEAT_HEAT_2D_HPP
