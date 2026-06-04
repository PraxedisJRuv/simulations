#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <algorithm>
#include <array>

struct Population{
    double S, I, R, N;

    Population(double s, double i, double r){
        S=s;  
        I=i; 
        R=r; 
        N=s+i+r;
    }

    friend std::ostream& operator<<(std::ostream& os, const Population& p){
        os << "|S =" << p.S
           << " | I =" << p.I
           << " | R =" << p.R
           << "\n";

        return os;
    }
    
    Population euler_slope(Population& p, double N, double beta, double gamma ){
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
            Population slope=euler_slope(s, N, beta, gamma);
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
            std::cout <<s<<"\n"; 
        }
        std::cout << "Approximate solution at time = " << T_end << " is " << s << "\n";
        return {s.S, s.I, s.R};
        
    }

    std::vector<double> get_sim_peaks(int M, Population& s, double T_end, double delta, double beta, double gamma){
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
            std::binomial_distribution<int> binom_I(s.S,1-std::exp((-beta*s.I*delta)/(N)));
            std::binomial_distribution<int> binom_R(s.I,1-std::exp((-gamma*delta)));
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

    
           
};

void print_peaks_p(std::vector<double>peaks){
        std::sort(peaks.begin(),peaks.end());
    int p5  = peaks[peaks.size()-1 * 0.05];
    int p50 = peaks[peaks.size()-1 * 0.50];
    int p95 = peaks[peaks.size()-1 * 0.95];
    std::cout<<"\n"<<peaks[p5]<<"\t"<<peaks[p50]<<"\t"<<peaks[p95]<<"\t"<<peaks[peaks.size()-1];
}

std::array<Population,3> euler_slope_age(std::array<Population,3> pop, std::array<std::array<float,3>,3> beta, std::array<double,3> gamma){
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

std::array<Population,3> change_age(double T_end, double delta, std::array<Population,3>& pop, std::array<std::array<float,3>,3> beta, std::array<double,3> gamma){
        double t=0;
        std::cout<<std::fixed<<std::setprecision(4);
        while(t<T_end){
            std::array<Population,3> slopes=euler_slope_age(pop, beta, gamma);
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
void test_age_groups(){
const int G = 3; 
std::array<std::array<float, G>, G> beta = {{
    {0.30, 0.10, 0.05},   
    {0.10, 0.25, 0.08},   
    {0.05, 0.08, 0.15}    
}};

std::array<Population,G> pop={{
    {1600,20,0},
    {4500,10,0},
    {3400,0,0}
}};

std::array<double,3> gamma={
{0.05,0.05,0.04}
};

std::array<Population,3> pop2 = {change_age(10,0.5,pop,beta,gamma)};
double icu_fraction=.10;
std::cout<<"ICU load is: \t"<<pop2[2].I*icu_fraction<<"\n";
}

void test_peaks(){
    Population pobla2(50,2,1);
    //std::cout<<pobla2;
    //pobla2.rnd_change(20,0.5, pobla2, 0.8, 0.3);

    //pobla2.S=50;
    //pobla2.I=2;
    //pobla2.R=1;
    
    std::vector<double> peaks=pobla2.get_sim_peaks(100,pobla2,100,0.5,0.8,0.3);
    print_peaks_p(peaks);
}

int main(){

    test_age_groups();
    
    Population pop(1600,20,0);
    pop.change(10,0.5,pop,0.15,0.04);
    double icu_fraction=.10;
    std::cout<<"ICU load is: \t"<<pop.I*icu_fraction;
    return 0;
}
