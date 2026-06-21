#pragma once
#include "FABM_sr_functions.hpp"

int test_main_(){
    int Amount=50;
    int Periods=100;
    BankParams param;
    std::vector<Banks> banks;
    
    for (int i=0; i<Amount; i++){
        banks.push_back(Banks(i, State::Solvent, param));
    }

    std::mt19937 rng(std::random_device{}());

    std::vector<std::vector<int>> contacts=conctacts_list(Amount, p, rng);
    std::vector<std::vector<double>> exposure=exposure_random(Amount, contacts, banks, param);

    trigger_default(banks, rng, Amount, 1);

    for (int j=0; j<Periods; j++){
        std::cout<<"Period: "<<j<<"\t |"
                 <<"Solvent: " <<count_state(banks,State::Solvent)<<"\t |"
                 <<"Stressed: "<<count_state(banks,State::Stressed)<<"\t |"
                 <<"Defaulted: "<<count_state(banks, State::Defaulted)<<"\n";
        update_state(banks, exposure, Amount);
    }
    return 0;
}