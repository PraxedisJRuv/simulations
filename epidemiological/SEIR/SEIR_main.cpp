#include "headers/SEIR_visualization.hpp"
#include "headers/SEIR_tests.hpp"

/*
State (S, E, I ,R)
Parameters (beta, gamma, sigma, vax)
*/
int main(){
    State s(100,2,2,0);
    Parameters p(0.4,0.1,0.6,0.001);
    double T=100;
    double delta=0.2;
    
    runVisualization(T,delta,s,p);

    return 0;
}