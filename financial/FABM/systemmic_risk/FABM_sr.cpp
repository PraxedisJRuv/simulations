#include <iostream>
#include <random>
#include <vector>
#include <random>

enum class State{Solvent, Stressed, Defaulted};

struct Banks{
    int id;
    double equity;
    double total_assets;
    State current_state;
    
    Banks(int id, State state){
        this->id=id;
        current_state=state;

        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<double>Z1(500,1500);
        std::uniform_int_distribution<int>Z2(0,300);

        equity=Z1(rng);
        total_assets=Z2(rng);
    }

    double capital_ratio()const{
        return equity/total_assets;
    }

    void classify(){
        double cr=capital_ratio();
        if(cr<= threshold_low) current_state=State::Defaulted;
        if(cr<=threshold_high) current_state=State::Stressed;
        else current_state=State::Solvent;
    }
};

/*
delta S is the Stress recovery rate
delta D the default recovery rate

the treshold high would be from solvent to stressed
the low would be from stressed to defaulted
*/
const double delta_S = 0.85;
const double delta_D = 0.40;
const double threshold_high = 0.08;
const double threshold_low  = 0.03;

const double p=0.1;
int N;

std::vector<std::vector<int>> conctacts_list(const int N, 
                                                const double p, 
                                                std::mt19937 rng){
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

std::vector<std::vector<double>> exposure_random(const int N, const std::vector<std::vector<int>>& contacts){
    std::vector<std::vector<double>> exposure_matrix(N);
    std::uniform_real_distribution<double>Z_debt(0,1500);
    for (int i=0; i<N; i++){
        int total_contacts=contacts[i].size();
        for (int j=0; j<total_contacts; j++){
            std::mt19937 rng(std::random_device{}());
            exposure_matrix[i].push_back(Z_debt(rng));
        }
    }
}
//define contagion function

double compute_loss(int id, const std::vector<Banks>& banks,
                    const std::vector<std::vector<double>>& exposure) {
    double loss = 0.0;
    int N = banks.size();
    for (int i = 0; i < N; i++) {
        if (i == id) continue;
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
    return loss;
}

