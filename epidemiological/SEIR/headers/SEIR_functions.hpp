#pragma once
#include "SEIR_structures.hpp"
#include <algorithm>

State solpe_euler(State& s, const Parameters& p){
            double new_S_infections=(p.beta*s.S*s.I)/s.N;
            double new_E_infections=(p.sigma*s.E);
            double new_recoveries=(p.gamma*s.I);
            double new_vaccinated=(p.vax*s.S);

            return{
                -new_S_infections-new_vaccinated,
                new_S_infections-new_E_infections,
                new_E_infections-new_recoveries,
                new_recoveries + new_vaccinated
            };
}


State change(const double T_end, const double delta, State& s, const Parameters& p){
        double t=0;
        std::cout<<std::fixed<<std::setprecision(4);
        while(t<T_end){
            State slope=solpe_euler(s, p);
            s.S=s.S+delta*slope.S;
            s.E=s.E+delta*slope.E;
            s.I=s.I+delta*slope.I;
            s.R=s.R+delta*slope.R;
            
            t=t+delta;
            std::cout<<"At t ="<<t<<"\t";
            std::cout << s; 
            }
            
        std::cout << "Approximate solution at time = " << T_end << " is " << s << "\n";
        return {s.S, s.E, s.I, s.R};
}


State rnd_change(const double T_end,const double delta, State& s, const Parameters& p){
        std::mt19937 rng(std::random_device{}());

        double t=0;
        while(t<T_end){
            std::binomial_distribution<int> binom_SR(s.S,1-std::exp((-p.vax*delta)));
            int new_vaxxed=binom_SR(rng);
            
            std::binomial_distribution<int> binom_SE(s.S-new_vaxxed,1-std::exp((-p.beta*s.I*delta)/(s.N)));
            std::binomial_distribution<int> binom_EI(s.E,1-std::exp((-p.sigma*delta)));
            std::binomial_distribution<int> binom_IR(s.I,1-std::exp((-p.gamma*delta)));
            
            int new_exposed=binom_SE(rng);
            int new_infected=binom_EI(rng);
            int new_recovered=binom_IR(rng);
            

            s.S=s.S-new_exposed-new_vaxxed;
            s.E=s.E+new_exposed-new_infected;
            s.I=s.I+new_infected-new_recovered;
            s.R=s.R+new_recovered+new_vaxxed;

            t=t+delta;
            std::cout<<"At t ="<<t<<"\t";
            std::cout <<s<<"\n"; 
        }
        std::cout << "Approximate solution at time = " << T_end << " is " << s << "\n";
        return {s.S, s.E, s.I, s.R};     
}


std::vector<double> get_sim_peaks(const int M, State& s, const double T_end, const double delta, const Parameters &p){
        std::mt19937 rng(std::random_device{}());
        double peak =0;
        double t=0;
        double N=s.S+s.I+s.R;
        std::vector<double> peaks(M,0);
        
        for (int i=0; i<M; i++){
            s.S=50;
            s.I=2;
            s.R=1;

            peak=0;
            t=0;
        while(t<T_end){

            std::binomial_distribution<int> binom_SR(s.S,1-std::exp((-p.vax*delta)));
            int new_vaxxed=binom_SR(rng);
            //the previous had to be done since both probabilities are independent in the same Population.
            std::binomial_distribution<int> binom_SE(s.S-new_vaxxed,1-std::exp((-p.beta*s.I*delta)/(s.N)));
            std::binomial_distribution<int> binom_EI(s.E,1-std::exp((-p.sigma*delta)));
            std::binomial_distribution<int> binom_IR(s.I,1-std::exp((-p.gamma*delta)));
            
            int new_exposed=binom_SE(rng);
            int new_infected=binom_EI(rng);
            int new_recovered=binom_IR(rng);
            

            s.S=s.S-new_exposed-new_vaxxed;
            s.E=s.E+new_exposed-new_infected;
            s.I=s.I+new_infected-new_recovered;
            s.R=s.R+new_recovered+new_vaxxed;

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