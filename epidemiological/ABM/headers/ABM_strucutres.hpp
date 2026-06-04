#pragma once
#include <iostream>

//States and intermidate states
enum class State{S,E1,E2,E3,I1,I2,R};
const int n_E=3;
const int n_I=2;

//operator override so State can be printed
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

/*
beta is the chance of becoming exposed after interacting with someone infected
gamma is the recovery rate, how much it takes to recover
sigma is the incubation rate, how much it takes for getting infected after being exposed
*/
struct Parameters{
    double beta, gamma, sigma;
    
    Parameters( double beta, double gamma, double sigma){
        this->beta=beta;
        this->gamma=gamma;
        this->sigma=sigma;
    }
};

struct Agent{
    int id;
    State state;
    double time_in_state;

};

