#pragma once
#include "FABM_sr_functions.hpp"
#include "FABM_sr_visualization.hpp"

int test_main_(){
    int Amount=50;
    int Periods=40;
    BankParams param;
    std::vector<Banks> banks;
    
    for (int i=0; i<Amount; i++){
        banks.push_back(Banks(i, State::Solvent, param));
    }

    std::mt19937 rng(std::random_device{}());

    std::vector<std::vector<int>> contacts=conctacts_list(Amount, p, rng);
    std::vector<std::vector<double>> exposure=exposure_random(Amount, contacts, banks, param);

    trigger_default(banks, rng, Amount, 1);
    runVisualization(banks, contacts, exposure, rng, param, Periods);
    return 0;
}