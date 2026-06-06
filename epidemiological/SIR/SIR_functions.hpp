#pragma once
#include "SIR_structures.hpp"
#include <algorithm>
#include <array>

State euler_slope(State& s, const Parameters& p){
    double new_infections=(p.beta*s.S*s.I)/s.N;
    double new_recoveries=(p.gamma*s.I);
        return{
            -new_infections,
            new_infections-new_recoveries,
            new_recoveries
        };
}


State change(double T_end, double delta, State& s, const Parameters& p){
        double t=0;
        std::cout<<std::fixed<<std::setprecision(4);
        while(t<T_end){
            State slope=euler_slope(s, p);
            s.S=s.S+delta*slope.S;
            s.I=s.I+delta*slope.I;
            s.R=s.R+delta*slope.R;
            
            t=t+delta;
            std::cout<<"At t ="<<t<<"\t";
            std::cout << s; 
            }
        std::cout << "Approximate solution at time = " << T_end << " is " << s << "\n";
    return {s.S, s.I, s.R};
}


 State rnd_change(double T_end, double delta, State& s, const Parameters& p){
        std::mt19937 rng(std::random_device{}());

        double t=0;
        while(t<T_end){
            std::binomial_distribution<int> binom_I(s.S,1-std::exp((-p.beta*s.I*delta)/(s.N)));
            std::binomial_distribution<int> binom_R(s.I,1-std::exp((-p.gamma*delta)));
            int new_infected=binom_I(rng);
            int new_recovered=binom_R(rng);

            s.S=s.S-new_infected;
            s.I=s.I+new_infected-new_recovered;
            s.R=s.R+new_recovered;

            t=t+delta;
            std::cout<<"At t ="<<t<<"\t";
            std::cout <<s<<"\n"; 
        }
        std::cout << "Approximate solution at time = " << T_end << " is " << s << "\n";
    return {s.S, s.I, s.R};    
}


std::vector<double> get_sim_peaks(int M, double T_end, double delta, State& s, const Parameters& p){
        std::mt19937 rng(std::random_device{}());
        double peak =0;
        double t=0;
        std::vector<double> peaks(M,0);
        
        for (int i=0; i<M; i++){
            s.S=50;
            s.I=2;
            s.R=1;

            peak=0;
            t=0;
        while(t<T_end){
            std::binomial_distribution<int> binom_I(s.S,1-std::exp((-p.beta*s.I*delta)/(s.N)));
            std::binomial_distribution<int> binom_R(s.I,1-std::exp((-p.gamma*delta)));
            int new_infected=binom_I(rng);
            int new_recovered=binom_R(rng);

            s.S=s.S-new_infected;
            s.I=s.I+new_infected-new_recovered;
            s.R=s.R+new_recovered;

            t=t+delta;
            peak=std::max(peak,s.I);
        }
        peaks[i]=peak;
        }    
    return peaks;
}


void print_peaks_p(std::vector<double>peaks){
        std::sort(peaks.begin(),peaks.end());
    int p5  = peaks[peaks.size()-1 * 0.05];
    int p50 = peaks[peaks.size()-1 * 0.50];
    int p95 = peaks[peaks.size()-1 * 0.95];
    std::cout<<"\n"<<peaks[p5]<<"\t"<<peaks[p50]<<"\t"<<peaks[p95]<<"\t"<<peaks[peaks.size()-1];
}


std::array<State,3> euler_slope_age(std::array<State,3> pop, std::array<std::array<float,3>,3> beta, std::array<double,3> gamma){
        double multiplier=0;
        double infected=0;
        double recovered=0;
        for (int i=0; i<3; i++){
            multiplier=0;
            for (int j=0; j<3; j++){
                multiplier=multiplier+(beta[i][j]*pop[j].I)/pop[j].N;
            }
            infected=pop[i].S*multiplier;
            recovered=gamma[i]*pop[i].I;

            pop[i].S=-infected;
            pop[i].I=infected-recovered;
            pop[i].R=recovered;
        }
    return pop;
}


std::array<State,3> change_age(double T_end, double delta, std::array<State,3>& pop, std::array<std::array<float,3>,3> beta, std::array<double,3> gamma){
        double t=0;
        std::cout<<std::fixed<<std::setprecision(4);
        while(t<T_end){
            std::array<State,3> slopes=euler_slope_age(pop, beta, gamma);
            for (int i=0; i<3; i++){
                pop[i].S=pop[i].S+delta*slopes[i].S;
                pop[i].I=pop[i].I+delta*slopes[i].I;
                pop[i].R=pop[i].R+delta*slopes[i].R;
                std::cout<<"At t ="<<t<<"\t";
                std::cout << pop[i]; 
            }            
            t=t+delta;
            }
        for (int i=0; i<3; i++){        
            std::cout << "Approximate solution at time = " << T_end << " is " << pop[i] << "\n";
            }

    return {pop};
}

