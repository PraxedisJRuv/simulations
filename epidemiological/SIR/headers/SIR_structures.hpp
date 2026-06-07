#pragma once
#include <iostream>
#include <iomanip>
#include <random>

struct State{
    double S, I, R, N;

    State(double s, double i, double r){
        S=s;  
        I=i; 
        R=r; 
        N=s+i+r;
    }

    friend std::ostream& operator<<(std::ostream& os, const State& s){
        os << "|S =" << s.S
           << " | I =" << s.I
           << " | R =" << s.R
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