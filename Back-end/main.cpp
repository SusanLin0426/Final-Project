#include "TimeContingentCashFlows.h"
#include "TermStructureHoLee.h"
#include "TermStructure.h"
#include "date.h"

#include <iostream>
#include <vector>
#include <algorithm>

// 要用的：
//  TermStructure
//  TermStructureHoLee
//  TimeContingentCashFlows
//  date
int main() {
    double delta = 0.98;
    double pi = 0.5;
    double r = 0.1;
    // input: K, maturity_date, callable_bond_information, coupon_rate, face_value, day_count_convention
    
    
    // maturity_date calculation
    int sYear = 2077, sMonth = 7, sDay = 7;//should be imputed by user or determined by the system
    date startingDate = date::current_date();
    int eYear = 2077, eMonth = 8, eDay = 8;//should be imputed by user
    date expirationDate(eDay, eMonth, eYear);

    if (!startingDate.valid() || !expirationDate.valid()) {
        //error message
        return 1;
    }

    int dcc_value = 0;
    // Determine the day count convention based on combox selection
    DayCountConvention dcc;
    switch (dcc_value) {//should be inputed by dropdown list
    case 0: dcc = DayCountConvention::Thirty360; break;
    case 1: dcc = DayCountConvention::Thirty365; break;
    case 2: dcc = DayCountConvention::Actual360; break;
    case 3: dcc = DayCountConvention::Actual365; break;
    case 4: dcc = DayCountConvention::ActualActual; break;
    default:
        //error message "Invalid day count convention selected");
        return 1;
    }
    // Calculate the time to maturity using years_until method
    double timeToMaturity = startingDate.years_until(expirationDate, dcc);

    //callable_bond_information
    //Enter face value of the bond, Enter coupon rate of the bond (as a decimal), Enter time to maturity of the bond (in years)
    double face_value = 100, coupon_rate = 0.05, time_to_maturity = 10;
    std::vector<double> underlying_bond_cflow_times;
    std::vector<double> underlying_bond_cflows;
    generate_bond_cash_flows(face_value, coupon_rate, time_to_maturity, underlying_bond_cflow_times, underlying_bond_cflows);

    // Flat term structure
    TermStructure* initial = new TermStructureFlat(r);
    std::vector<double> times;
    times.push_back(5.0);
    std::vector<double> cflows;
    cflows.push_back(100);
    double K = 80;
    time_to_maturity = 3;
    std::cout << "Flat term structure" << std::endl;
    std::cout << "c = " << price_european_call_option_on_bond_using_ho_lee(initial, delta, pi, times, cflows, K, time_to_maturity) << std::endl;
    std::cout << std::endl;

    delete initial;
    // Nelson-Siegel term structure
    double beta0 = 0.09;
    double beta1 = 0.01;
    double beta2 = 0.01;
    double lambda = 5.0;
    TermStructure::NelsonSiegelParams ns_params(beta0, beta1, beta2, lambda);
    initial = new TermStructureInterpolated();

    // Setting up the Nelson-Siegel term structure with interpolated observations
    std::vector<double> ns_times = { 1.0, 2.0, 3.0, 4.0, 5.0 }; // Example times
    std::vector<double> ns_yields;
    for (double t : ns_times) {
        ns_yields.push_back(TermStructure::nelsonSiegelSpotRate(t, ns_params));
    }
    dynamic_cast<TermStructureInterpolated*>(initial)->setInterpolatedObservations(ns_times, ns_yields);

    std::cout << "Nelson Siegel term structure" << std::endl;
    std::cout << "c = " << price_european_call_option_on_bond_using_ho_lee(initial, delta, pi, times, cflows, K, time_to_maturity) << std::endl;
    std::cout << std::endl;

    delete initial;

    return 0;
}
