#include <iostream>
#include <random>
#include <vector>
#include <random>

enum class State{Solvent, Stressed, Defaulted};

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

/*
delta S is the Stress recovery rate
delta D the default recovery rate

the treshold high would be from solvent to stressed
the low would be from stressed to defaulted
*/


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

std::vector<std::vector<double>> exposure_random(const int N, const std::vector<std::vector<int>>& contacts, std:: vector<Banks>& banks, const BankParams &p){
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

void trigger_default(std::vector<Banks>& banks, const int& id){
    banks[id].equity=0.0;
    banks[id].current_state=State::Defaulted;
}

void print_equity(std::vector<Banks>& banks, const int& N){
    for (int i=0; i<N; i++){
        std::cout<<banks[i].equity<<" ";
    }
    std::cout<<"\n";
}

int main(){
    int Amount=50;
    int Periods=100;
    BankParams param;
    std::vector<Banks> banks;
    for (int i=0; i<Amount; i++){
        banks.push_back(Banks(i, State::Solvent, param));
    }

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> pick(0,Amount-1);
    int trigger_id=pick(rng);

    std::vector<std::vector<int>> contacts=conctacts_list(Amount, p, rng);
    std::vector<std::vector<double>> exposure=exposure_random(Amount, contacts, banks, param);

    trigger_default(banks, trigger_id);

    for (int j=0; j<Periods; j++){
        std::cout<<"Period: "<<j<<"\t |"
                 <<"Solvent: " <<count_state(banks,State::Solvent)<<"\t |"
                 <<"Stressed: "<<count_state(banks,State::Stressed)<<"\t |"
                 <<"Defaulted: "<<count_state(banks, State::Defaulted)<<"\n";
        update_state(banks, exposure, Amount);
    }
    return 0;
}