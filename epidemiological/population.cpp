#include <iostream>
#include <iomanip>

struct Population{
    double S, I, R;

    Population(double s, double i, double r){
        S=s;  // Susceptible
        I=i; // Infected
        R=r; // Recovered
    }

    friend std::ostream& operator<<(std::ostream& os, const Population& p){
        os << "S =" << p.S
           << " | R =" << p.R
           << " | I =" << p.I
           << "\n";

        return os;
    }
    
    Population deriv(Population& p, double N, double beta, double gamma ){
            double new_infections=(beta*p.S*p.I)/N;
            double new_recoveries=(gamma*p.I);

            return{
                -new_infections,
                new_infections-new_recoveries,
                new_recoveries
            };

    }

    Population change(double T_end, double delta, Population& s, double N, double beta, double gamma){
        double t=0;
        std::cout<<std::fixed<<std::setprecision(4);
        while(t<T_end){
            Population slope=deriv(s, N, beta, gamma);
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

};


int main(){

    Population pobla(50,2,1);
    std::cout<<pobla;
    std::cout<<pobla.deriv(pobla,56,3,2);
    pobla.change(10,0.5,pobla, 2, .02, .03);
    std::cout<<pobla;
    return 0;
}
