#include <iostream>
#include <vector>
#include <random>
#include <cmath>


/*
simga is the incubation rate
gamma the recovery rate
*/
enum class State{S,E1,E2,E3,I1,I2,R};
const int n_E=3;
const int n_I=2;

std::ostream& operator<<(std::ostream& os, State state) {
    switch (state) {
        case State::S:  os <<"S"; break;
        case State::E1: os <<"E1"; break;
        case State::E2: os <<"E2"; break;
        case State::E3:  os <<"E3"; break;
        case State::I1: os <<"I1"; break;
        case State::I2: os <<"I2"; break;
        case State::R: os <<"R"; break;
        default:           os << "Unknown State"; break;
    }
    return os;
}
struct Parameters{
    float beta, gamma, sigma;
    Parameters( float beta, float gamma, float sigma){
        this->beta=beta;
        this->gamma=gamma;
        this->sigma=sigma;
    }
};

struct Agent{
    int id;
    State state;
    int time_in_state;

    void transition(Agent& a, std::mt19937& rng, Parameters param, double delta,
        std::bernoulli_distribution advance_E, std::bernoulli_distribution advance_I){
        
        switch(a.state){
            case State::S:{
                double prob=1.0-std::exp(-param.beta*delta);
                if(std::bernoulli_distribution(prob)(rng)){
                    a.state=State::E1;}
                }break;
            case State::E1:
                if(advance_E(rng)){
                    a.state=State::E2;
                }break;
            case State::E2:
                if(advance_E(rng)){
                    a.state=State::E3;
                }break;
            case State::E3:
                if(advance_E(rng)){
                    a.state=State::I1;
                }break;
            case State::I1:
                if(advance_I(rng)){
                    a.state=State::I2;
                }break;
            case State::I2:
                if(advance_I(rng)){
                    a.state=State::R;
                } break;
            
            default: break;
        }

    }
};

int count(std::vector<Agent> agents, State I){
    int count =0;
    for (int i=0; i<agents.size(); i++){
        if (agents[i].state==I){
            count=count+1;
        }
    }
    return count;
}

void test_random_progress(){
    double delta=0.5;
    Parameters param(0.5,0.2,0.1);
    float rate_E=n_E*param.sigma;
    float rate_I=n_I*param.gamma;
    
    std::mt19937 rng(std::random_device{}());

    std::bernoulli_distribution advance_E(1-std::exp(-rate_E*delta));
    std::bernoulli_distribution advance_I(1-std::exp(-rate_I*delta));

    std::vector<Agent> agents={{1,State::S,0},{2,State::I1,1},{3,State::I1,0}};

    for (int j=0; j<10; j++){
    for (int i=0; i<agents.size(); i++){
        agents[i].transition(agents[i], rng, param, delta, advance_E, advance_I);
        std::cout<<agents[i].state<<"\t";
    }
        std::cout<<j<<"\n";
    }


}

int main(){
    //Agent ej1={1,State::S,0};
    test_random_progress();
    return 0;
}