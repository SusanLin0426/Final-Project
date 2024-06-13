#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "TermStructure.h"
#include "TermStructureHoLee.h"
#include "TimeContingentCashFlows.h"
#include "date.h"

// source: https://github.com/yhirose/cpp-httplib
#include "httplib.h"

// source: https://github.com/nlohmann/json
// reference: https://hackmd.io/@tico88612/cpp-json-file-tutorial?print-pdf#/
#include "json.hpp"

using namespace std;
using namespace httplib;
using json = nlohmann::json;

// Define a structure for storing CSV data
struct CSVRow {
    vector<string> columns;
};

// Function to parse a CSV row
CSVRow parseCSVRow(const string &line) {
    CSVRow row;
    stringstream ss(line);
    string column;
    while (getline(ss, column, ',')) {
        row.columns.push_back(column);
    }
    return row;
}

// Function to read CSV and extract data
void readCSV(const string& filename, vector<double>& times, vector<double>& yields) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file" << endl;
        return;
    }

    string line;
    // Skip the header
    getline(file, line);
    while (getline(file, line)) {
        CSVRow row = parseCSVRow(line);
        if (row.columns.size() >= 2) {
            times.push_back(stod(row.columns[0]));
            yields.push_back(stod(row.columns[1]));
        }
    }
    file.close();
}

// CORS headers
void setup_cors_headers(Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Accept, Content-Type, Content-Length, Accept-Encoding, X-CSRF-Token, Authorization");
}

int main() {

    Server svr;

    svr.Options("/.*", [](const Request &req, Response &res) {
        setup_cors_headers(res);
        res.status = 200; // No content
    });

    svr.Post("/calculate", [](const Request& req, Response& res) {

        setup_cors_headers(res);

        auto params = json::parse(req.body);

        vector<double> times_;
        vector<double> yields_;

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
            return 1;
        }

        // Determine the day count convention based on combox selection

        int dcc_case = 0;
        DayCountConvention dcc;

        switch (dcc_case) {//should be imputed by dropdown list
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
        vector<double> underlying_bond_cflow_times;
        vector<double> underlying_bond_cflows;
        generate_bond_cash_flows(face_value, coupon_rate, time_to_maturity, underlying_bond_cflow_times, underlying_bond_cflows);

        vector<double> times;
        times.push_back(5.0);
        vector<double> cflows;
        cflows.push_back(100);
        double K = 80;
        // double time_to_maturity = 3;

        vector<double> market_times;
        market_times.push_back(1.0);
        market_times.push_back(2.0);
        market_times.push_back(3.0);
        market_times.push_back(4.0);
        market_times.push_back(5.0);

        vector<double> market_prices;
        market_prices.push_back(0.95);
        market_prices.push_back(0.90);
        market_prices.push_back(0.85);
        market_prices.push_back(0.80);
        market_prices.push_back(0.75);


        // cout << "cubic term structure" << endl;
        cout << price_european_call_option_on_bond_using_ho_lee(initial, 
                                                                delta, 
                                                                pi, 
                                                                underlying_bond_cflow_times, 
                                                                underlying_bond_cflows, 
                                                                K, 
                                                                timeToMaturity) << endl;
        // cout << endl;

        delete initial;

        json response;
        // response["straight_bond_price"] = interest_rate_trees_gbm_value_of_cashflows(cashflows, tree, q);
        response["callable_bond_price"] = "";

        res.set_content(response.dump(), "application/json");
    });

    cout << "Server is running at http://localhost:3001" << endl;
    svr.listen("localhost", 3001);

    return 0;
}


