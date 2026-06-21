#pragma once
#include "FABM_sr_structures.hpp"

int count_state(std::vector<Banks>& banks, State s){
    int N=banks.size();
    int count=0;
    for (int i=0; i<N; i++){
        if (s!=banks[i].current_state){
            continue;
        }
        count++;
    }
    return count;
}

std::vector<std::vector<int>> conctacts_list(const int& N, const double& p, std::mt19937& rng){
    std::vector<std::vector<int>> contacts(N);
    std::bernoulli_distribution edge(p);
    for (int i=0; i<N; i++){
        for(int j=i+1; j<N; j++){
            if(edge(rng)){
                contacts[i].push_back(j);
                contacts[j].push_back(i);
            }
        }
    }
    return contacts;
}

std::vector<std::vector<double>> exposure_random(const int& N, const std::vector<std::vector<int>>& contacts, std:: vector<Banks>& banks, const BankParams& p){
    std::vector<std::vector<double>> exposure_matrix(N);
    std::uniform_real_distribution<double>Z_debt(0.02,p.max_exposure_fraction);
    for (int i=0; i<N; i++){
        int total_contacts=contacts[i].size();
        for (int j=0; j<total_contacts; j++){
            std::mt19937 rng(std::random_device{}());
            exposure_matrix[i].push_back(banks[i].equity*Z_debt(rng));
        }
    }
    return exposure_matrix;
}

double compute_loss(int id, const std::vector<Banks>& banks,
                     std::vector<std::vector<double>>& exposure, int const &N) {
    double loss = 0.0;
    for (int i=0; i<N; i++){
        int total_contacts=exposure[i].size();
        for (int j=0; j<total_contacts; j++){
            if (j!=id) continue;
            
            double owed = exposure[i][id]; // bank i owes bank id this amount
            
            if (owed <= 0.0) continue;

            double recovery = 0.0;
            switch (banks[i].current_state) {
                case State::Solvent:   recovery = 1.0;     break;
                case State::Stressed:  recovery = delta_S; break;
                case State::Defaulted: recovery = delta_D; break;
            }
            loss = loss+ (owed * (1.0 - recovery));
            }
        }
    return loss;
}

void update_state(std::vector<Banks>& banks, std::vector<std::vector<double>>& exposure, const int& N){
    for (int i=0; i<N; i++){
        if (banks[i].current_state==State::Defaulted){
            continue;
        }
        double loss = compute_loss(i, banks, exposure, N);
        banks[i].equity=banks[i].equity-loss;
        banks[i].classify();
    }
}

void trigger_default(std::vector<Banks>& banks, std::mt19937& rng, const int& N, const int& total_defaults){
    std::uniform_int_distribution<int> pick(0,N-1);
    for (int i=0; i<total_defaults; i++){
        int id =pick(rng);
        banks[id].equity=0.0;
        banks[id].current_state=State::Defaulted;
    }  
}

