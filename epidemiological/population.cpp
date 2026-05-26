#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <algorithm>

struct Population{
    double S, I, R;

    Population(double s, double i, double r){
        S=s;  
        I=i; 
        R=r; 
    }

    friend std::ostream& operator<<(std::ostream& os, const Population& p){
        os << "S =" << p.S
           << " | I =" << p.I
           << " | R =" << p.R
           << "\n";

        return os;
    }
    
    Population det_deriv(Population& p, double N, double beta, double gamma ){
            double new_infections=(beta*p.S*p.I)/N;
            double new_recoveries=(gamma*p.I);

            return{
                -new_infections,
                new_infections-new_recoveries,
                new_recoveries
            };

    }

    Population change(double T_end, double delta, Population& s, double beta, double gamma){
        double N=s.S+s.I+s.R;
        double t=0;
        std::cout<<std::fixed<<std::setprecision(4);
        while(t<T_end){
            Population slope=det_deriv(s, N, beta, gamma);
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

    Population rnd_change(double T_end, double delta, Population& s, double beta, double gamma){
        std::mt19937 rng(std::random_device{}());

        double N=s.S+s.I+s.R;
        double t=0;
        while(t<T_end){
            std::binomial_distribution<int> binom_I(s.S,1-std::exp((-beta*s.I*delta)/(N)));
            std::binomial_distribution<int> binom_R(s.I,1-std::exp((-gamma*delta)));
            int new_infected=binom_I(rng);
            int new_recovered=binom_R(rng);

            s.S=s.S-new_infected;
            s.I=s.I+new_infected-new_recovered;
            s.R=s.R+new_recovered;

            t=t+delta;
            std::cout<<"At t ="<<t<<"\t";
            std::cout << s; 
        }
        std::cout << "Approximate solution at time = " << T_end << " is " << s << "\n";
        return {s.S, s.I, s.R};
        
    }

    std::vector<double> get_sim_peaks(int M, Population& s, double T_end, double delta, double beta, double gamma){
        std::mt19937 rng(std::random_device{}());
        double peak =0;
        double N=s.S+s.I+s.R;
        std::vector<double> peaks(M,0);
        
        for (int i=0; i<M; i++){
            double t=0;
            s.S=50;
            s.I=2;
            s.R=1;
        while(t<T_end){
            std::binomial_distribution<int> binom_I(s.S,1-std::exp((-beta*s.I*delta)/(N)));
            std::binomial_distribution<int> binom_R(s.I,1-std::exp((-gamma*delta)));
            int new_infected=binom_I(rng);
            int new_recovered=binom_R(rng);

            s.S=s.S-new_infected;
            s.I=s.I+new_infected-new_recovered;
            s.R=s.R+new_recovered;

            t=t+delta;
            
            peak=std::max(peak,s.I);
            std::cout << peak;
        }
        peaks.push_back(peak);
        peak=0;
        }
        
        return peaks;
    }
           
};


int main(){

    Population pobla(50,2,1);
    std::cout<<pobla;
    //std::cout<<pobla.det_deriv(pobla,56,3,2);
    //pobla.change(10,0.5,pobla, 2, .02, .03, "det");
    //std::cout<<pobla;
    pobla.change(10,0.5, pobla, 0.2, 0.3);
    Population pobla2(50,2,1);
    std::cout<<pobla2;
    pobla2.rnd_change(10,0.5, pobla2, 0.2, 0.3);

    pobla2.S=50;
    pobla2.I=2;
    pobla2.R=1;
    
    pobla2.get_sim_peaks(100,pobla2,10,0.5,0.2,0.3);


    return 0;
}
