#include "functions.h"

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
        transition(agents[i], rng, param, delta, advance_E, advance_I);
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

void test_progress_with_network(){
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