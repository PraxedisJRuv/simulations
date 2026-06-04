#pragma once
#include"ABM_strucutres.hpp"
#include <random>

/*
advance_E is the probability function of progressing substates Ek until state I is reach
Analog for advance_I but from the first substate Im until state R
*/
void transition(Agent& a, std::mt19937& rng, 
                const Parameters param, const double& delta, 
                std::bernoulli_distribution& advance_E,
                std::bernoulli_distribution& advance_I){
    
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


//counts the population at a specific state
int count(const std::vector<Agent>& agents, State I){
    int count = 0;
    for (int i = 0; i < static_cast<int>(agents.size()); i++){
        if (agents[i].state == I){
            count += 1;
        }
    }
    return count;
}

/*
generates a random contacts_list for each node (person in the system)
p is the probability of two people knowing each other,
this is a very simplified version, further models could have diferent
functions to determine the contacts_list
*/
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


//How many contacts a person i has
int degree(int i, const std::vector<std::vector<int>>& c){
    return c[i].size();
}


int neighbors_in_state(int i, 
                        const std::vector<std::vector<int>>& c,
                        const std::vector<Agent>& a,
                        State s){
    int count =0;
    for (int j=0; j<c[i].size(); j++){
        if(a[c[i][j]].state==s){
            count = count + 1;
        }
    }
    return count;
}


inline bool isInfectious(State s) {
    return s == State::I1 || s == State::I2;
}

int infectious_neighbors(int i, 
                        const std::vector<std::vector<int>>& contacts, 
                        const std::vector<Agent>& agents){
    int count = 0;
    for (int j=0; j<contacts[i].size(); j++){
        if (isInfectious(agents[contacts[i][j]].state)){
            count = count + 1;
        }
    }
    return count;
}

 void transition_with_network(int i, 
                                std::vector<Agent> &a, 
                                Agent & agent, 
                                const std::vector<std::vector<int>> &c, 
                                std::mt19937& rng, 
                                const Parameters &param, 
                                const double &delta,
                                std::bernoulli_distribution advance_E, 
                                std::bernoulli_distribution advance_I){
        
        switch(agent.state){
            case State::S: {
                double prob=1.0-std::exp(-param.beta*delta);
                int exposure = infectious_neighbors(i, c, a);
                for (int j=0; j<exposure; j++){
                    if(std::bernoulli_distribution(prob)(rng)){
                        agent.state = State::E1;
                        agent.time_in_state = 0;
                        break;
                    }
                }
                if (agent.state != State::E1){
                    agent.time_in_state = agent.time_in_state + delta;
                }
            } break;

            case State::E1:
                if (advance_E(rng)){
                    agent.state = State::E2;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;

            case State::E2:
                if (advance_E(rng)){
                    agent.state = State::E3;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;

            case State::E3:
                if (advance_E(rng)){
                    agent.state = State::I1;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;

            case State::I1:
                if (advance_I(rng)){
                    agent.state = State::I2;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;

            case State::I2:
                if (advance_I(rng)){
                    agent.state = State::R;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;

            default:
                agent.time_in_state = agent.time_in_state + delta;
                break;
        }
}

void agents_progress_with_network(int &N, int& T, 
                                    std::vector<Agent>& agents,
                                    const std::vector<std::vector<int>>& contacts,
                                    std::mt19937& rng,
                                    const Parameters &param, 
                                    double &delta, 
                                    std::bernoulli_distribution &advance_E,
                                    std::bernoulli_distribution &advance_I){
                                    
    std::vector<Agent> agents_copy;
    for (int j=0; j<T; j++){
        agents_copy=agents;
        for (int i=0; i<N; i++){
            transition_with_network(i, agents_copy, agents[i],contacts,
                                    rng,param, delta, advance_E,advance_I);
        }
    }
}

void simulateDay(std::vector<Agent>& agents,
                 const std::vector<std::vector<int>>& contacts,
                 std::mt19937& rng,
                 const Parameters& param,
                 double delta,
                 std::bernoulli_distribution& advance_E,
                 std::bernoulli_distribution& advance_I){
    std::vector<Agent> next = agents;
    int N = static_cast<int>(agents.size());
    for (int i = 0; i < N; i++){
        transition_with_network(i, agents, next[i], contacts, rng, param, delta, advance_E, advance_I);
    }
    agents.swap(next);
}

