#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "TermStructure.h"
#include "TermStructureHoLee.h"
#include "TimeContingentCashFlows.h"
#include "date.h"

// Define a structure for storing CSV data
struct CSVRow {
    std::vector<std::string> columns;
};

// Function to parse a CSV row
CSVRow parseCSVRow(const std::string &line) {
    CSVRow row;
    std::stringstream ss(line);
    std::string column;
    while (std::getline(ss, column, ',')) {
        row.columns.push_back(column);
    }
    return row;
}

// Function to read CSV and extract data
void readCSV(const std::string& filename, std::vector<double>& times, std::vector<double>& yields) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return;
    }

    std::string line;
    // Skip the header
    std::getline(file, line);
    while (std::getline(file, line)) {
        CSVRow row = parseCSVRow(line);
        if (row.columns.size() >= 2) {
            times.push_back(std::stod(row.columns[0]));
            yields.push_back(std::stod(row.columns[1]));
        }
    }
    file.close();
}


int main() {
    std::vector<double> times_;
    std::vector<double> yields_;

    // Read data from CSV
    readCSV("maturity_days_and_rates.csv", times_, yields_);

    // cubic term structure
    TermStructure* initial = new TermStructureInterpolated();
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
        return 0 ;
    }

    // Determine the day count convention based on combox selection
    DayCountConvention dcc;
    switch (true) {//should be imputed by dropdown list
    case 0: dcc = DayCountConvention::Thirty360; break;
    case 1: dcc = DayCountConvention::Thirty365; break;
    case 2: dcc = DayCountConvention::Actual360; break;
    case 3: dcc = DayCountConvention::Actual365; break;
    case 4: dcc = DayCountConvention::ActualActual; break;
    default:
        //error message "Invalid day count convention selected");
        return 0;
    }
    // Calculate the time to maturity using years_until method
    double timeToMaturity = startingDate.years_until(expirationDate, dcc);

    //callable_bond_information
    //Enter face value of the bond, Enter coupon rate of the bond (as a decimal), Enter time to maturity of the bond (in years)
    double face_value = 100, coupon_rate = 0.05, time_to_maturity = 10;
    std::vector<double> underlying_bond_cflow_times;
    std::vector<double> underlying_bond_cflows;
    generate_bond_cash_flows(face_value, coupon_rate, time_to_maturity, underlying_bond_cflow_times, underlying_bond_cflows);

    std::vector<double> times;
    times.push_back(5.0);
    std::vector<double> cflows;
    cflows.push_back(100);
    double K = 80;
    // double time_to_maturity = 3;

    std::vector<double> market_times;
    market_times.push_back(1.0);
    market_times.push_back(2.0);
    market_times.push_back(3.0);
    market_times.push_back(4.0);
    market_times.push_back(5.0);

    std::vector<double> market_prices;
    market_prices.push_back(0.95);
    market_prices.push_back(0.90);
    market_prices.push_back(0.85);
    market_prices.push_back(0.80);
    market_prices.push_back(0.75);


    // std::cout << "cubic term structure" << std::endl;
    std::cout << price_european_call_option_on_bond_using_ho_lee(initial, delta, pi, underlying_bond_cflow_times, underlying_bond_cflows, K, timeToMaturity, market_times, market_prices) << std::endl;
    // std::cout << std::endl;

    delete initial;

    return 0;
}


