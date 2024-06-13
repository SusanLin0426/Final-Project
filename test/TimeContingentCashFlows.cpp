//TimeContingentCashFlows.cpp
#include "TimeContingentCashFlows.h"
#include "TermStructureHoLee.h"
#include "TermStructure.h"
#include <algorithm>
#include <vector>
#include <iostream>

std::vector<TimeContingentCashFlows> build_time_series_of_bond_time_contingent_cash_flows(const std::vector<double>& initial_times, 
                                                                                          const std::vector<double>& initial_cflows) {
    std::vector<TimeContingentCashFlows> vec_cf;
    std::vector<double> times = initial_times;
    std::vector<double> cflows = initial_cflows;

    while (times.size() > 0) {
        vec_cf.push_back(TimeContingentCashFlows(times, cflows));
        std::vector<double> tmp_times;
        std::vector<double> tmp_cflows;
        for (int i = 0; i < times.size(); ++i) {
            if (times[i] - 1.0 >= 0.0) {
                tmp_times.push_back(times[i] - 1.0);
                tmp_cflows.push_back(cflows[i]);
            }
        }
        times = tmp_times;
        cflows = tmp_cflows;
    }

    return vec_cf;
}

double price_european_call_option_on_bond_using_ho_lee(TermStructure* initial,
                                                        const double& delta, 
                                                        const double& pi, 
                                                        const std::vector<double>& underlying_bond_cflow_times, 
                                                        const std::vector<double>& underlying_bond_cflows,
                                                        const double& K, 
                                                        const double& option_time_to_maturity) {

    int T = int(option_time_to_maturity + 0.0001);
    auto hl_tree = buildTermStructureTree(initial, T + 2, delta, pi);   
    auto vec_cf = build_time_series_of_bond_time_contingent_cash_flows(underlying_bond_cflow_times, underlying_bond_cflows);

    for (size_t i = 0; i < hl_tree.size(); ++i) { // time i
        const auto& row = hl_tree[i];
        for (size_t j = 0; j < row.size(); ++j) { // statue j
            const auto& node = row[j];
            //std::cout << "Discount factor at row " << i << " node " << j << " is " << node.d(1.0) << std::endl;
        }
    }
    

    // Print vec_cf and T for debugging
    //std::cout << "T: " << T << std::endl;
    //std::cout << "vec_cf:" << std::endl;
    for (const auto& cf : vec_cf) {
        cf.print();
    }

    //std::cout << "vec_cf.sizez():" << vec_cf.size() << std::endl;
    //std::cout << "underlying_bond_cflows.sizez():" << underlying_bond_cflows.size() << std::endl;

    // Ensure T + 1 is within bounds
    if (T + 1 >= vec_cf.size()) {
        std::cerr << "Error: T + 1 exceeds the size of vec_cf." << std::endl;
        return -1;  // Return an error code or handle it appropriately
    }

    std::vector<double> values(T + 1, 0.0);
    for (int i = 0; i <= T; ++i) {
        values[i] = std::max(0.0, bonds_price(vec_cf[T + 1].times, vec_cf[T + 1].cash_flows, hl_tree[T][i]) - K); // HL: T + 1 ? 
        //std::cout << "check i :" << i << std::endl;
    }

    for (int t = T - 1; t >= 0; --t) {
        std::vector<double> values_this(t + 1, 0.0);
        for (int i = 0; i <= t; ++i) {
            values_this[i] = (pi * values[i + 1] + (1.0 - pi) * values[i]) * hl_tree[t][i].d(1);
        }
        values = values_this;
    }

    return values[0];
}


