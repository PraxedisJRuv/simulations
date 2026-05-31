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
    float time_in_state;

    void transition(Agent& a, std::mt19937& rng, Parameters param, double delta,
        std::bernoulli_distribution advance_E, std::bernoulli_distribution advance_I){
        
        switch(a.state){
            case State::S:{
                double prob=1.0-std::exp(-param.beta*delta);
                if(std::bernoulli_distribution(prob)(rng)){
                    a.state=State::E1;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                }break;
            case State::E1:
                if(advance_E(rng)){
                    a.state=State::E2;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            case State::E2:
                if(advance_E(rng)){
                    a.state=State::E3;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            case State::E3:
                if(advance_E(rng)){
                    a.state=State::I1;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            case State::I1:
                if(advance_I(rng)){
                    a.state=State::I2;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            case State::I2:
                if(advance_I(rng)){
                    a.state=State::R;
                    a.time_in_state=0;} 
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            
            default: 
                a.time_in_state=a.time_in_state+delta;
                break;
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

std::vector<std::vector<int>> conctacts_list(int N, float p, std::mt19937 rng){
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

int degree(int i, std::vector<std::vector<int>>& c){
return c[i].size();
}

int neighbors_in_state(int i, std::vector<std::vector<int>> &c, std::vector<Agent> &a, State I){
    int count=0;
    for (int j=0; j<c[i].size(); j++){
        if(a[c[i][j]].state==I){
            count = count+1;
        }
    }
    return count;
}

 void transition_with_network(int i, std::vector<Agent> &a, Agent & agent, std::vector<std::vector<int>> &c, 
                                std::mt19937& rng, const Parameters &param, const double &delta,
                                std::bernoulli_distribution advance_E, std::bernoulli_distribution advance_I){
        
        switch(agent.state){
            case State::S:{
                double prob=1.0-std::exp(-param.beta*delta);
                int exposure= neighbors_in_state(i, c, a, State::I1);
               
                for (int j=0; j<exposure; j++){
                    if(std::bernoulli_distribution(prob)(rng)){
                    agent.state=State::E1;
                    agent.time_in_state=0;
                    break;}
                }
                if(agent.state!=State::E1){agent.time_in_state=agent.time_in_state+delta;}
                }break;
            case State::E1:
                if(advance_E(rng)){
                    agent.state=State::E2;
                    agent.time_in_state=0;}
                else{agent.time_in_state=agent.time_in_state+delta;}
                break;
            case State::E2:
                if(advance_E(rng)){
                    agent.state=State::E3;
                    agent.time_in_state=0;}
                else{agent.time_in_state=agent.time_in_state+delta;}
                break;
            case State::E3:
                if(advance_E(rng)){
                    agent.state=State::I1;
                    agent.time_in_state=0;}
                else{agent.time_in_state=agent.time_in_state+delta;}
                break;
            case State::I1:
                if(advance_I(rng)){
                    agent.state=State::I2;
                    agent.time_in_state=0;}
                else{agent.time_in_state=agent.time_in_state+delta;}
                break;
            case State::I2:
                if(advance_I(rng)){
                    agent.state=State::R;
                    agent.time_in_state=0;} 
                else{agent.time_in_state=agent.time_in_state+delta;}
                break;
            
            default: 
                agent.time_in_state=agent.time_in_state+delta;
                break;
        }

    }

std::vector<std::vector<int>> test_contact_list(){
    int N=200;
    float p=0.05;

    double mean=0;
    int max=0;
    int min=300;

    int aux=0;
    int sum=0;

    std::mt19937 rng(std::random_device{}());
    std::vector<std::vector<int>> contacts=conctacts_list(N, p, rng);
    for (int i=0; i<N; i++){
        aux=degree(i,contacts);
        sum=sum+aux;
        if(aux>max){max=aux;}
        if(aux<min){min=aux;}
    }
    mean=sum/N;

    std::cout<<min<<"\t"<<max<<"\t"<<mean<<"\n";
    
    return contacts;
}

std::vector<Agent> test_random_progress(){
    double delta=0.5;
    int N=200;
    Parameters param(0.2,0.01,0.4);
    float rate_E=n_E*param.sigma;
    float rate_I=n_I*param.gamma;
    
    std::mt19937 rng(std::random_device{}());

    std::bernoulli_distribution advance_E(1-std::exp(-rate_E*delta));
    std::bernoulli_distribution advance_I(1-std::exp(-rate_I*delta));

    std::vector<Agent> agents;
    for (int i=0; i<197; i++){
        agents.push_back({i,State::S,0});
    }
    for (int i=197; i<200; i++){
        agents.push_back({i,State::I1,0});
    }

    for (int j=0; j<30; j++){
    for (int i=0; i<agents.size(); i++){
        agents[i].transition(agents[i], rng, param, delta, advance_E, advance_I);
    }
   
        std::cout<<"S: "<<count(agents,State::S)
                 <<"\t E1:"<<count(agents,State::E1)
                 <<"\t E2:"<<count(agents,State::E2)
                 <<"\t E3:"<<count(agents,State::E3)
                 <<"\t I1:"<<count(agents,State::I1)
                 <<"\t I2:"<<count(agents,State::I2)
                 <<"\t R:"<<count(agents,State::R)
                 <<"\t";
        std::cout<<j<<"\n"; 

    }
    return agents;
}


void progress_with_network(){
    double delta=0.5;
    int N=200;
    Parameters param(0.2,0.01,0.4);
    float rate_E=n_E*param.sigma;
    float rate_I=n_I*param.gamma;
    
    std::mt19937 rng(std::random_device{}());

    std::bernoulli_distribution advance_E(1-std::exp(-rate_E*delta));
    std::bernoulli_distribution advance_I(1-std::exp(-rate_I*delta));

    std::vector<Agent> agents;
    for (int i=0; i<197; i++){
        agents.push_back({i,State::S,0});
    }
    for (int i=197; i<200; i++){
        agents.push_back({i,State::I1,0});
    }

    
    float p_contact=0.05;
    std::vector<std::vector<int>> contacts=conctacts_list(N, p_contact, rng);

    std::vector<Agent> agents_copy;
    for (int j=0; j<30; j++){
        agents_copy=agents;
        for (int i=0; i<N; i++){
            transition_with_network(i, agents_copy, agents[i],contacts,
                                    rng,param, delta, advance_E,advance_I);
        }
            std::cout<<"S: "<<count(agents,State::S)
                 <<"\t E1:"<<count(agents,State::E1)
                 <<"\t E2:"<<count(agents,State::E2)
                 <<"\t E3:"<<count(agents,State::E3)
                 <<"\t I1:"<<count(agents,State::I1)
                 <<"\t I2:"<<count(agents,State::I2)
                 <<"\t R:"<<count(agents,State::R)
                 <<"\t";
            std::cout<<j<<"\n"; 
    }
}

int main(){
    //Agent ej1={1,State::S,0};
    progress_with_network();
    test_random_progress();
    return 0;
}