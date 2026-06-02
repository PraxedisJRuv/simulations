#pragma once
#include "pc_functions.hpp"

int test(const Sim_Paramaeters& sim, PriceContainer &s, const Asset_Parameters& p, std::mt19937& rng, double percentile){
    //std::vector<PriceContainer> paths=simulateBGM(test,0.1,0.3,1,15,100);
    double mc_price=mc_european_call_payoff(sim, s, p, rng);
    double bs_price=black_scholes(sim, s, p);
    double error =std::abs(mc_price -bs_price);
    double errorpercent=error/bs_price;
    std::cout<<"European call payoff"<<"\t"<<"Black Scholes payoff"<<"\t"<<"Error"<<"\t"<<"Error percent"<<"%"<<"\n";
    std::cout<<mc_price<<"\t"<<bs_price<<"\t"<<error<<"\t"<<errorpercent<<"%"<<"\n";
    std::vector<PriceContainer> paths=simulateBGM(sim, s, p, rng);
    std::cout<<"\n"<<"VaR"<<VaR(sim, paths, percentile)<<"\t\t at "<<percentile;
    return 0;
}

void cholesky_test(const Sim_Paramaeters& sim, double w1, double w2, double rho, const Asset_Parameters& p1,const Asset_Parameters& p2, std::mt19937& rng){

    std::vector <double> choleskyport=cholesky_GBM_r(sim, w1, w2, rho, p1, p2, rng);
    std::sort(choleskyport.begin(),choleskyport.end());
    double var_1pct  = choleskyport[sim.M * 0.01];
    double var_5pct  = choleskyport[sim.M * 0.05];

    double mean = 0.0;
    
    for (double p : choleskyport) {
        mean = mean + p;
    }
    mean /=sim.M;

    std::cout << "mean daily return : " << mean*100 << "%\n"
              << "1% VaR \t\t: " << var_1pct*100 << "%\n"
              << "5% VaR \t\t: " << var_5pct*100 << "%\n";

}