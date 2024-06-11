#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class TermStructureClass {
public:
    virtual double d(const double& T) const = 0;
};

class InitialTerm : public TermStructureClass {
public:
    double d(const double& T) const override {
        return 0.025; // Initial rate is 0.025
    }
};

class TermStructureClassHoLee : public TermStructureClass {
private:
    TermStructureClass* initial_term;
    int n;
    int i;
    double delta;
    double pi;

public:
    TermStructureClassHoLee(TermStructureClass* fitted_term, const int& n,
                                const int& i, const double& lambda, const double& pi);
    double d(const double& T) const;
};

TermStructureClassHoLee::TermStructureClassHoLee(TermStructureClass* fitted_term,
                                                           const int& n, const int& i,
                                                           const double& lambda, const double& pi) {
    initial_term = fitted_term;
    this->n = n;
    this->i = i;
    this->delta = lambda;
    this->pi = pi;
}

double TermStructureClassHoLee::d(const double& T) const { 
    double d = (*initial_term).d(T + n) / (*initial_term).d(n); 
    for (int j = 1; j < n; ++j) {
        d *= (1.0 / (pi + (1 - pi) * pow(delta, T + (n - j)))) / (1.0 / (pi + (1 - pi) * pow(delta, n - j)));
    } 
    d *= (1.0 / (pi + (1 - pi) * pow(delta, T))) * pow(delta, T * (n - i));
    return d; 
}

vector<vector<TermStructureClassHoLee> >
build_term_structure_tree(TermStructureClass* initial,
                          const int& no_steps,
                          const double& delta,
                          const double& pi) {
    vector<vector<TermStructureClassHoLee> > hl_tree;
    for (int t = 0; t < no_steps; ++t) {
        hl_tree.push_back(vector<TermStructureClassHoLee>());
        for (int j = 0; j <= t; ++j) {
            TermStructureClassHoLee hl(initial, t, j, delta, pi);
            hl_tree[t].push_back(hl);
        }
    }
    return hl_tree;
}

class CallableBond {
private:
    vector<double> cashflows;
    vector<double> redemption_times;
    vector<double> redemption_values;

public:
    CallableBond(const vector<double>& cfs, const vector<double>& rt, const vector<double>& rv);
    double price(const TermStructureClassHoLee& term_structure) const;
};

CallableBond::CallableBond(const vector<double>& cfs, const vector<double>& rt, const vector<double>& rv) {
    cashflows = cfs;
    redemption_times = rt;
    redemption_values = rv;
}

double CallableBond::price(const TermStructureClassHoLee& term_structure) const {
    double bond_price = 0.0;
    for (size_t i = 0; i < redemption_times.size(); ++i) {
        double present_value = 0.0;
        for (size_t j = 0; j < cashflows.size(); ++j) {
            if (redemption_times[i] >= j) {
                present_value += cashflows[j] * term_structure.d(redemption_times[i]);
            }
        }
        bond_price += min(present_value, redemption_values[i] * term_structure.d(redemption_times[i]));
    }
    return bond_price;
}

int main() {

    TermStructureClass* initial = new InitialTerm();
 
    // input: no_steps, delta, pi
    // reference: Financial Recipe 2014 chap 25
    int no_steps = 5;       
    double delta = 0.25;    
    double pi = 0.05;       

    vector<vector<TermStructureClassHoLee> > hl_tree = build_term_structure_tree(initial, no_steps, delta, pi);

    for (int t = 0; t < no_steps; ++t) {
        for (int j = 0; j <= t; ++j) {
            cout << "d at time " << t << ", index " << j << ": " << hl_tree[t][j].d(t) << endl;
        }
    }

    TermStructureClassHoLee term_structure(initial, no_steps, 0, delta, pi);

    vector<double> cashflows;
    cashflows.push_back(5.0);
    cashflows.push_back(5.0);
    cashflows.push_back(5.0);
    cashflows.push_back(105.0);

    vector<double> redemption_times;
    redemption_times.push_back(1.0);
    redemption_times.push_back(2.0);
    redemption_times.push_back(3.0);
    redemption_times.push_back(4.0);

    vector<double> redemption_values;
    redemption_values.push_back(100.0);
    redemption_values.push_back(100.0);
    redemption_values.push_back(100.0);
    redemption_values.push_back(100.0);
    CallableBond bond(cashflows, redemption_times, redemption_values);

    double bond_price = bond.price(term_structure);

    cout << "==================================" << endl;
    cout << "Callable bond price: " << bond_price << endl;

    delete initial;

    return 0;
}
