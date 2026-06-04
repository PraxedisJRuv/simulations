#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <iomanip>

struct State{
    double S, E, I, R, N;

    State(double s, double e, double i, double r){
        S=s; 
        E=e; 
        I=i; 
        R=r;
        N=s+e+i+r;
    }

    friend std::ostream& operator<<(std::ostream& os, const State& p){
        os << "|S =" << p.S
           << " | E ="<<p.E
           << " | I =" << p.I
           << " | R =" << p.R
           << "\n";

        return os;
    }
     
};

struct Parameters{
    double beta, gamma, sigma, vax;
    Parameters(double beta, double gamma, double sigma, double vax){
        this-> beta=beta;
        this-> gamma=gamma;
        this-> sigma=sigma;
        this-> vax=vax;
    }
};
