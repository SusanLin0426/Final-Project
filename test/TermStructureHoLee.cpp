//TermStructureHoLee.cpp
#include "TermStructureHoLee.h"
#include "TermStructure.h"
#include <cmath>
#include <vector>
#include <iostream>

TermStructureHoLee::TermStructureHoLee(TermStructure* fitted_term, const int& n, const int& i, const double& delta, const double& pi)
    : initial_term_(fitted_term), n_(n), i_(i), delta_(delta), pi_(pi) {}

inline double hT(const double& T, const double& delta, const double& pi) {
    return (1.0 / (pi + (1 - pi) * pow(delta, T)));
}

double TermStructureHoLee::d(const double& T) const {
    double d = initial_term_->d(T + n_) / initial_term_->d(n_);


    for (int j = 1; j < n_; ++j) {
        double hT1 = hT(T + (n_ - j), delta_, pi_);
        double hT2 = hT(n_ - j, delta_, pi_);
        d *= hT1 / hT2;

    }

    double hT_final = hT(n_, delta_, pi_);
    double pow_delta = pow(delta_, T * (n_ - i_));
    d *= hT_final * pow_delta;

    return d;
}


std::vector<std::vector<TermStructureHoLee> > buildTermStructureTree(TermStructure* initial, 
                                                                    const int& no_steps, 
                                                                    const double& delta, 
                                                                    const double& pi) {
    std::vector<std::vector<TermStructureHoLee> > hl_tree;

    for (int t = 0; t < no_steps; ++t) {
        hl_tree.push_back(std::vector<TermStructureHoLee>());
        for (int j = 0; j <= t; ++j) {
            TermStructureHoLee hl(initial, t, j, delta, pi); 
            hl_tree[t].push_back(hl);
        }
    }
    return hl_tree;
}

void TermStructureHoLee::print(const size_t& row, const size_t& node) const {
    std::cout << "1 year Discount factor at row " << row << " node " << node << " is " << d(1.0) << std::endl;
}

//example
//#include "TermStructureHoLee.h"
//#include "TermStructure.h"
//#include <iostream>
//#include <vector>
//
//int main() {
//    std::vector<double> times = { 0.1, 1, 5 };
//    std::vector<double> spotrates = { 0.025, 0.025, 0.025 };
//
//    int no_steps = 30;
//    double delta = 0.98;
//    double pi = 0.5;
//    double r = 0.1;
//    TermStructureFlat initial_term(r);
//
//
//    auto hl_tree = buildTermStructureTree(&initial_term, no_steps, delta, pi);
//
//    for (size_t i = 0; i < hl_tree.size(); ++i) { // time i
//        const auto& row = hl_tree[i];
//        for (size_t j = 0; j < row.size(); ++j) { // statue j
//            const auto& node = row[j];
//            std::cout << "Discount factor at row " << i << " node " << j << " is " << node.d(1.0) << std::endl;
//        }
//    }
//
//    // Example usage of pricing function (not fully implemented)
//    std::vector<double> bond_cflow_times = { 1, 2, 3, 4, 5 };
//    std::vector<double> bond_cflows = { 5, 5, 5, 5, 105 };
//    double K = 100;
//    double option_time_to_maturity = 1.0;
//
//    double option_price = priceEuropeanCallOptionOnBondUsingHoLee(&initial_term, delta, pi, bond_cflow_times, bond_cflows, K, option_time_to_maturity);
//    std::cout << "Option price: " << option_price << std::endl;
//
//    return 0;
//}
