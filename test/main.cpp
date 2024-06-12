#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "TermStructure.h"
#include "TermStructureHoLee.h"
#include "TimeContingentCashFlows.h"

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
    double delta = 0.98;
    double pi = 0.5;
    double r = 0.1;

    TermStructure* initial = new TermStructureInterpolated();

    std::vector<double> times;
    times.push_back(5.0);
    std::vector<double> cflows;
    cflows.push_back(100);
    double K = 80;
    double time_to_maturity = 3;
    // // Setting up the Nelson-Siegel term structure with interpolated observations


    std::cout << "cubic term structure" << std::endl;
    std::cout << "c = " << price_european_call_option_on_bond_using_ho_lee(initial, delta, pi, times, cflows, K, time_to_maturity) << std::endl;
    std::cout << std::endl;

    delete initial;

    return 0;
}



