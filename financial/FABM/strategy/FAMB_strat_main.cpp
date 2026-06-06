#include "headers/FABM_strat_tests.hpp"

int main(){

    std::vector<Trader> t;
    
    for (int i=0; i<2; i++){
        t.push_back(Trader(i, Strategy::Fundamentalist));
    }
    for (int i=0; i<2; i++){
        t.push_back(Trader(i+34, Strategy::Chartist));
    }
    for (int i=0; i<2; i++){
        t.push_back(Trader(i+67, Strategy::Random));
    }

    PriceContainer price({10,11,11,12,13,13.2,13.7,14.1});

    double fair_value=13.5;
    double lambda=0.1;
    int T=100;

    desired_action_test(t, price, fair_value, lambda, T);

    return 0;
}