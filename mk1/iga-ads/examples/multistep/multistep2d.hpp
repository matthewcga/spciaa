// SPDX-FileCopyrightText: 2015 - 2023 Marcin Łoś <marcin.los.91@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef MULTISTEP_MULTISTEP2D_HPP
#define MULTISTEP_MULTISTEP2D_HPP

#include "ads/executor/galois.hpp"
#include "ads/output_manager.hpp"
#include "ads/simulation.hpp"
#include "ads/util/ring.hpp"
#include "multistep_base.hpp"

namespace ads::problems {

class multistep2d : public simulation_2d, public multistep_base {
private:
    using Base = simulation_2d;

    util::ring<vector_type> us;

    lin::band_matrix Ax, Ay;
    lin::solver_ctx Ax_ctx, Ay_ctx;

    output_manager<2> output;
    galois_executor executor{8};

public:
    multistep2d(const config_2d& config, scheme scm, int order)
    : Base{config}
    , multistep_base{std::move(scm), order}
    , us{std::max(s + 1, 2), shape()}
    , Ax{x.p, x.p, x.dofs()}
    , Ay{y.p, y.p, y.dofs()}
    , Ax_ctx{Ax}
    , Ay_ctx{Ay}
    , output{x.B, y.B, 200} { }

private:
    void prepare_spaces() {
        x.fix_left();
        x.fix_right();
        y.fix_left();
        y.fix_right();
    }

    void prepare_matrices() {
        double eta = bs[0] * steps.dt;

        fill_matrix(Ax, x.basis, eta);
        fill_matrix(Ay, y.basis, eta);

        fix_dof(0, x, Ax);
        fix_dof(x.dofs() - 1, x, Ax);
        fix_dof(0, y, Ay);
        fix_dof(y.dofs() - 1, y, Ay);

        lin::factorize(Ax, Ax_ctx);
        lin::factorize(Ay, Ay_ctx);

        Base::prepare_matrices();
    }

    void print_errors(const vector_type& u, double t) const {
        std::cout << " error " << errorL2(u, t) << " " << errorH1(u, t);
        std::cout << " norm " << norm(u, x, y, L2{}) << " " << norm(u, x, y, H1{});
        std::cout << std::endl;
    }

    void before() override {
        prepare_spaces();
        prepare_matrices();

        int needed = us.size() - 1;
        for (int i = 0; i < needed; ++i) {
            double t = i * steps.dt;
            auto init = [this, t](double x, double y) { return solution(x, y, t).val; };

            projection(us[0], init);
            apply_bc(us[0]);
            solve(us[0]);

            us.rotate();
        }
        us.rotate();
    }

    void before_step(int /*iter*/, double /*t*/) override { us.rotate(); }

    void step(int iter, double t) override {
        if (iter < us.size() - 2) {
            return;
        }
        compute_rhs(us[0], t);
        apply_bc(us[0]);

        ads_solve(us[0], buffer, dim_data{Ax, Ax_ctx}, dim_data{Ay, Ay_ctx});

        adjust_solution(us);
    }

    void after() override {
        double t = steps.dt * steps.step_count;
        std::cout << steps.step_count << " " << t << " ";
        print_errors(us[0], t);
    }

    void after_step(int iter, double t) override {
        double tt = t + steps.dt;
        int ii = iter + 1;

        // Output
        if (ii % 1000 == 0) {
            output.to_file(us[0], "out_%d.data", iter);
        }

        // Error & norm reporting
        if (ii % 10 == 0) {
            std::cout << ii << " " << tt << " ";
            print_errors(us[0], tt);
        }
    }

    void compute_rhs(vector_type& rhs, double t) {
        zero(rhs);

        executor.for_each(elements(), [&](index_type e) {
            auto U = element_rhs();
            std::vector<value_type> uvals(us.size());

            double tau = steps.dt;
            double tt = t + tau;
            double eta = bs[0] * tau;

            double J = jacobian(e);
            for (auto q : quad_points()) {
                double w = weight(q);
                auto x = point(e, q);

                for (int i = 1; i < us.size(); ++i) {
                    uvals[i] = eval_fun(us[i], e, q);
                }

                for (auto a : dofs_on_element(e)) {
                    auto aa = dof_global_to_local(e, a);
                    value_type v = eval_basis(e, q, a);

                    double val = 0;

                    for (int i = 0; i <= s; ++i) {
                        double ti = tt - i * tau;
                        val += tau * bs[i] * force(x, ti) * v.val;
                    }

                    auto const form = [this](auto u, auto v) { return grad_dot(u, v); };

                    for (int i = 1; i <= s; ++i) {
                        auto u = uvals[i];
                        val -= as[i - 1] * u.val * v.val + tau * bs[i] * form(u, v);
                    }
                    if (s == 0) {
                        val -= as[s] * uvals[s + 1].val * v.val;
                    }

                    // Correction to get higher order
                    // We apply M + eta K to solutions from previous time steps and add it
                    // to RHS, multiplied by finite difference coefficients
                    for (int i = 1; i < order; ++i) {
                        auto u = uvals[i];
                        val += fibo[i] * (u.val * v.val + eta * form(u, v));
                    }

                    U(aa[0], aa[1]) += val * w * J;
                }
            }

            executor.synchronized([&]() { update_global_rhs(rhs, U, e); });
        });
    }

    auto exact(double t) const {
        return [t, this](point_type x) { return solution(x[0], x[1], t); };
    }

    double errorL2(const vector_type& u, double t) const {
        return error_relative(u, x, y, L2{}, exact(t)) * 100;
    }

    double errorH1(const vector_type& u, double t) const {
        return error_relative(u, x, y, H1{}, exact(t)) * 100;
    }

    // Problem definition

    double force(point_type /*x*/, double /*t*/) const { return 0; }

    void apply_bc(vector_type& rhs) {
        for_boundary_dofs(x, y, [&](index_type i) { rhs(i[0], i[1]) = 0; });
    }

    value_type solution(double x, double y, double t) const {
        constexpr double k = 2 * M_PI * M_PI;
        double e = std::exp(-k * t);

        return value_type{e * std::sin(x * M_PI) * std::sin(y * M_PI),
                          e * M_PI * std::cos(x * M_PI) * std::sin(y * M_PI),
                          e * M_PI * std::sin(x * M_PI) * std::cos(y * M_PI)};
    }
};

}  // namespace ads::problems

#endif  // MULTISTEP_MULTISTEP2D_HPP
