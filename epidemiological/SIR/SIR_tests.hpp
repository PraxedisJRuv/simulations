#pragma once
#include "SIR_functions.hpp"

void test_age_groups(){
const int G = 3; 
std::array<std::array<float, G>, G> beta = {{
    {0.30, 0.10, 0.05},   
    {0.10, 0.25, 0.08},   
    {0.05, 0.08, 0.15}    
}};

std::array<State,G> pop={{
    {1600,20,0},
    {4500,10,0},
    {3400,0,0}
}};

std::array<double,3> gamma={
{0.05,0.05,0.04}
};

std::array<State,3> pop2 = {change_age(10,0.5,pop,beta,gamma)};
double icu_fraction=.10;
std::cout<<"ICU load is: \t"<<pop2[2].I*icu_fraction<<"\n";
}

void test_peaks(){
    State s(50,2,1);
    Parameters p(0.8,0.3,0, 0);
    //std::cout<<pobla2;
    //pobla2.rnd_change(20,0.5, pobla2, 0.8, 0.3);

    //pobla2.S=50;
    //pobla2.I=2;
    //pobla2.R=1;
    
    std::vector<double> peaks=get_sim_peaks(100,100,0.5, s,p);
    print_peaks_p(peaks);
}