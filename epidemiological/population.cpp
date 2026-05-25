#include <iostream>

struct Population{
    double S, I, R;
    Population(double s, double i, double r){
        S=s;
        I=i;
        R=r;
    }

};

void pop_from_read_csv(){

}

std::ostream& operator<<(std::ostream& os, const Population& p){
    os <<"S ="<<p.S
       <<" | R ="<<p.R
       <<" | I ="<<p.I;

    return os;
}
