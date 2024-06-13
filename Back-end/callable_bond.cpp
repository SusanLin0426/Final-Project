#include <vector>
#include <cmath>
#include <iostream>

// source: https://github.com/yhirose/cpp-httplib
#include "httplib.h"

// source: https://github.com/nlohmann/json
// reference: https://hackmd.io/@tico88612/cpp-json-file-tutorial?print-pdf#/
#include "json.hpp"

using namespace std;
using namespace httplib;
using json = nlohmann::json;

vector<vector<double> > interest_rate_trees_gbm_build(const double& r0, const double& u, const double& d, const int& n) {
    vector<vector<double> > tree;
    vector<double> r(1, r0);
    tree.push_back(r);

    for (int i = 1; i <= n; ++i) {
        double rtop = r[r.size() - 1] * u;
        for (int j = 0; j < i; ++j) {
            r[j] = d * r[j];
        }
        r.push_back(rtop);
        tree.push_back(r);
    }

    return tree;
}

double interest_rate_trees_gbm_value_of_cashflows(const vector<double>& cflow, const vector<vector<double> >& r_tree, double q) {
    int n = cflow.size();
    vector<vector<double> > values(n, vector<double>(n, 0.0));

    // 初始化最後一期價值
    for (int i = 0; i < n; ++i) {
        values[n - 1][i] = cflow[n - 1];
    }

    // 遞迴算每期價值
    for (int t = n - 2; t >= 0; --t) {
        for (int i = 0; i <= t; ++i) {
            values[t][i] = cflow[t] + exp(-r_tree[t][i]) * (q * values[t + 1][i] + (1 - q) * values[t + 1][i + 1]);
        }
    }

    return values[0][0];
}

double interest_rate_trees_gbm_value_of_callable_bond(const vector<double>& cflows,
                                                      const vector<vector<double> >& r_tree,
                                                      double q,
                                                      int first_call_time,
                                                      double call_price) {
    int n = cflows.size();
    vector<vector<double> > values(n);
    vector<double> value(n);

    // 初始化最後一期的價值
    for (int i = 0; i < n; ++i) {
        value[i] = cflows[n - 1];
    }
    values[n - 1] = value;

    // 遞迴計算每期的價值
    for (int t = n - 1; t > 0; --t) {
        vector<double> value(t, 0.0);
        for (int i = 0; i < t; ++i) {
            value[i] = cflows[t - 1] + exp(-r_tree[t - 1][i]) * (q * values[t][i] + (1 - q) * values[t][i + 1]);
            // 檢查是否達到可贖回時間，如果是則價值取現值和贖回價格中的較小者
            if (t >= first_call_time) {
                value[i] = min(value[i], call_price);
            }
        }
        values[t - 1] = value;
    }

    return values[0][0];
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

        double r0 = params["r0"];
        double u = params["u"];
        double d = params["d"];
        int n = params["n"];
        double q = params["q"];
        
        // for callable bond price
        int first_call_time = 6;
        double call_price = params["call_price"];

        vector<vector<double>> tree = interest_rate_trees_gbm_build(r0, u, d, n);

        vector<double> cashflows;
        cashflows.push_back(0);
        for (int t = 1; t <= 9; ++t) {
            cashflows.push_back(6);
        }
        cashflows.push_back(call_price);

        json response;
        // response["straight_bond_price"] = interest_rate_trees_gbm_value_of_cashflows(cashflows, tree, q);
        response["callable_bond_price"] = interest_rate_trees_gbm_value_of_callable_bond(cashflows, tree, q, first_call_time, call_price);

        res.set_content(response.dump(), "application/json");
    });

    cout << "Server is running at http://localhost:3002" << endl;
    svr.listen("localhost", 3002);

    return 0;
}
