#include "SEIR_functions.hpp"

void test_peaks(double T, double delta, State& s, Parameters& p){
    //std::cout<<pobla2;
    //pobla2.rnd_change(20,0.5, pobla2, 0.8, 0.3);
    State s2=s;
    State s3=s;
    change(T,delta,s, p);

    rnd_change(10,.05,s2, p);

    std::vector<double> peaks=get_sim_peaks(100,s3,10,0.5, p);
    
    print_peaks_p(peaks);
}