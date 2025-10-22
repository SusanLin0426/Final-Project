#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "date.h"
#include "date.cpp"
#include "TermStructure.h"
#include "TermStructure.cpp"
#include "TermStructureHoLee.h"
#include "TermStructureHoLee.cpp"
#include "TimeContingentCashFlows.h"
#include "TimeContingentCashFlows.cpp"

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
    
    // svr.Post("/sumbit", [](const Request& req, Response& res) {
    //     setup_cors_headers(res);

    //     auto params = json::parse(req.body);

    //     // Check if it's a multipart/form-data
    //     if (req.has_file("json")) {
    //         const auto& file = req.get_file_value("json");

    //         // Assuming file.content contains the JSON data
    //         json params = json::parse(file.content);
            
    //     } 
    //     else {
    //         res.status = 400;
    //         res.set_content("JSON file part 'json' not found in the request.", "text/plain");
    //     }
    // });

    svr.Post("/calculate", [](const Request& req, Response& res) {
        setup_cors_headers(res);

        auto params = json::parse(req.body);

        // get json data fron front-end
        double K = params["k"];
        string maturity_date = params["maturity_date"];

        double delta = params["delta"];
        double pi = params["pi"];
        double coupon_rate = params["coupon_rate"];
        double face_value = params["face_value"];
        int dcc_case = stoi(params["day_count_convention"]["value"].get<string>());

        // split maturity date
        int eYear, eMonth, eDay;
        sscanf(maturity_date.c_str(), "%d-%d-%d", &eYear, &eMonth, &eDay);

        // check values
        cout << "K: " << K << endl;
        cout << "maturity_date: " << maturity_date << endl;
        cout << "eYear: " << eYear << endl;
        cout << "eMonth: " << eMonth << endl;
        cout << "eDay: " << eDay << endl;
        cout << "delta: " << delta << endl;
        cout << "pi: " << pi << endl;
        cout << "coupon_rate: " << coupon_rate << endl;
        cout << "face_value: " << face_value << endl;
        cout << "day_count_convention: " << dcc_case << endl;

        vector<double> times_;
        vector<double> yields_;
        
        // upload_file
        // Read data from CSV
        readCSV("maturity_days_and_rates.csv", times_, yields_);

        // cubic term structure
        // TermStructure* initial = new TermStructureInterpolated();

        TermStructureInterpolated *initial = new TermStructureInterpolated(times_, yields_);
        
        date startingDate = date::current_date();
        date expirationDate(eDay, eMonth, eYear);

        if (!startingDate.valid() || !expirationDate.valid()) {
            //error message
            return 1;
        }

        // Determine the day count convention based on combox selection
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

        cout << "timeToMaturity: " << timeToMaturity << endl;
        //callable_bond_information
        //Enter face value of the bond, Enter coupon rate of the bond (as a decimal), Enter time to maturity of the bond (in years)
        //double face_value = 100, coupon_rate = 0.05, 
        
        double time_to_maturity = 10;
        vector<double> underlying_bond_cflow_times;
        vector<double> underlying_bond_cflows;
        generate_bond_cash_flows(face_value, coupon_rate, time_to_maturity, underlying_bond_cflow_times, underlying_bond_cflows);

        vector<double> times;
        times.push_back(5.0);
        vector<double> cflows;
        cflows.push_back(100);

        json response;

        vector<double> getTime = initial->getTimes();
        vector<double> getDiscountFactor = initial->getDiscountFactors();

        double callable_bond_price = price_european_call_option_on_bond_using_ho_lee(initial,
                                                                                    delta, 
                                                                                    pi, 
                                                                                    underlying_bond_cflow_times,
                                                                                    underlying_bond_cflows, 
                                                                                    K, 
                                                                                    timeToMaturity,
                                                                                    getTime,
                                                                                    getDiscountFactor);

        // test
        cout << "callable bond price: " << callable_bond_price << endl;

        response["callable_bond_price"] = callable_bond_price;
        
        delete initial;
        res.set_content(response.dump(), "application/json");
    });

    cout << "Server is running at http://localhost:3001" << endl;
    svr.listen("localhost", 3001);

    return 0;
}


