#pragma once
#include <iostream>
#include <random>

/*
The following are parameters needes to be fixed for execution but that
should be adjusted dependind on the conditions of the problem.
*/
const double delta_S = 0.85;
const double delta_D = 0.40;
const double threshold_high = 0.08;
const double threshold_low  = 0.03;

const double p=0.3;

struct BankParams {
    double total_assets_min = 100.0, total_assets_max = 500.0;
    double capital_ratio_min = 0.08, capital_ratio_max = 0.15;      
    double max_exposure_fraction = 0.20; 
};

enum class State{Solvent, Stressed, Defaulted};

struct Banks{
    int id;
    double equity;
    double total_assets;
    State current_state;
    
    Banks(int id, State state, const BankParams& p){
        this->id=id;
        this->current_state=state;

        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<double>Z1(p.capital_ratio_min,p.capital_ratio_max);
        std::uniform_int_distribution<int>Z2(p.total_assets_min,p.total_assets_max);

        total_assets=Z2(rng);
        equity=total_assets*Z1(rng);
    }

    double capital_ratio()const{
        return equity/total_assets;
    }

    void classify(){
        double cr=capital_ratio();
        if(cr<= threshold_low) current_state=State::Defaulted;
        else if(cr<=threshold_high) current_state=State::Stressed;
        else current_state=State::Solvent;
    }
};