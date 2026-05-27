#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <algorithm>

/*
S is suceptible
E is Exposed
I is Infected
R is Recovered

beta is the infection rate for S regarding I
gamma is the recovery rate for I
sigma is the infection rate for E
vax is the vacciantion rate for S

*/

struct Population{
    double S, E, I, R, N;

    Population(double s, double e, double i, double r){
        S=s; 
        E=e; 
        I=i; 
        R=r;
        N=s+e+i+r;
    }

    friend std::ostream& operator<<(std::ostream& os, const Population& p){
        os << "|S =" << p.S
           << " | E ="<<p.E
           << " | I =" << p.I
           << " | R =" << p.R
           << "\n";

        return os;
    }
    
    Population det_deriv(Population& p, float beta, float gamma, float sigma, float vax){
            double new_S_infections=(beta*p.S*p.I)/p.N;
            double new_E_infections=(sigma*p.E);
            double new_recoveries=(gamma*p.I);
            double new_vaccinated=(vax*p.S);

            return{
                -new_S_infections-new_vaccinated,
                new_S_infections-new_E_infections,
                new_E_infections-new_recoveries,
                new_recoveries + new_vaccinated
            };

    }

    Population change(double T_end, double delta, Population& p, float beta, float gamma, float sigma, float vax){
        double t=0;
        std::cout<<std::fixed<<std::setprecision(4);
        while(t<T_end){
            Population slope=det_deriv(p, beta, gamma, sigma, vax);
            p.S=p.S+delta*slope.S;
            p.E=p.E+delta*slope.E;
            p.I=p.I+delta*slope.I;
            p.R=p.R+delta*slope.R;
            
            t=t+delta;
            std::cout<<"At t ="<<t<<"\t";
            std::cout << p; 
            }
        std::cout << "Approximate solution at time = " << T_end << " is " << p << "\n";
        return {p.S, p.E, p.I, p.R};
        
    }

    Population rnd_change(double T_end, double delta, Population& s, float beta, float gamma, float sigma, float vax){
        std::mt19937 rng(std::random_device{}());

        double t=0;
        while(t<T_end){
            std::binomial_distribution<int> binom_SR(s.S,1-std::exp((-vax*delta)));
            int new_vaxxed=binom_SR(rng);
            
            std::binomial_distribution<int> binom_SE(s.S-new_vaxxed,1-std::exp((-beta*s.I*delta)/(s.N)));
            std::binomial_distribution<int> binom_EI(s.E,1-std::exp((-sigma*delta)));
            std::binomial_distribution<int> binom_IR(s.I,1-std::exp((-gamma*delta)));
            
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

    std::vector<double> get_sim_peaks(int M, Population& s, double T_end, double delta, float beta, float gamma, float sigma, float vax){
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

            std::binomial_distribution<int> binom_SR(s.S,1-std::exp((-vax*delta)));
            int new_vaxxed=binom_SR(rng);
            //the previous had to be done since both probabilities afre independent in the same population.
            std::binomial_distribution<int> binom_SE(s.S-new_vaxxed,1-std::exp((-beta*s.I*delta)/(s.N)));
            std::binomial_distribution<int> binom_EI(s.E,1-std::exp((-sigma*delta)));
            std::binomial_distribution<int> binom_IR(s.I,1-std::exp((-gamma*delta)));
            
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

    
           
};

void print_peaks_p(std::vector<double>peaks){
        std::sort(peaks.begin(),peaks.end());
    int p5  = peaks[peaks.size()-1 * 0.05];
    int p50 = peaks[peaks.size()-1 * 0.50];
    int p95 = peaks[peaks.size()-1 * 0.95];
    std::cout<<"\n"<<peaks[p5]<<"\t"<<peaks[p50]<<"\t"<<peaks[p95]<<"\t"<<peaks[peaks.size()-1];
}
int main(){

    
    //std::cout<<pobla2;
    //pobla2.rnd_change(20,0.5, pobla2, 0.8, 0.3);

    
    Population pobla(50,4,2,1);
    pobla.change(10,.05,pobla,0.5,0.3,0.9,0.01);

    pobla.S=50;
    pobla.E=4;
    pobla.I=2;
    pobla.R=1;

    pobla.rnd_change(10,.05,pobla,0.5,0.3,0.9,0.01);


    Population pobla2(50,4,2,1);
    std::vector<double> peaks=pobla2.get_sim_peaks(100,pobla2,10,0.5,0.5,0.3,.9,.01);
    print_peaks_p(peaks);
    return 0;
}
