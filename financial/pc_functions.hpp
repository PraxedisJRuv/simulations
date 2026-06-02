#pragma once
#include "price_container.hpp"
#include <random>

double normalCDF(double x){
    return 0.5*std::erfc(-x/std::sqrt(2.0));
}

double flat_yield(PriceContainer stock, float t, double r){
    return stock.current_price()*std::exp(r*t);
}

std::vector<PriceContainer> simulateBGM(const Sim_Paramaeters& sim, PriceContainer& s, const Asset_Parameters& p, std::mt19937& rng){
    std::vector<double> vec(sim.n,0);
    std::vector<PriceContainer> paths(sim.M,PriceContainer(vec));
    std::normal_distribution<double> Z(0,1);
    double delta = static_cast<double>(sim.T)/static_cast<double>(sim.n);
    double multiplier=0;
    int last=(int)(s.close).size();

    for (int i=0; i<sim.M; i++){
        paths[i].close[0]=s.close[last-1];
    }

    for (int j=1; j<sim.n; j++){
        for (int i=0; i<sim.M; i++){
            multiplier = std::exp(p.mu*delta + p.vola*std::sqrt(delta)*Z(rng));
            paths[i].close[j] = (paths[i].close[j-1]) * multiplier;
        }
    }
    return paths;
}

std::vector<PriceContainer> simulateBGM_rn(const Sim_Paramaeters& sim, PriceContainer& s, const Asset_Parameters& p, std::mt19937& rng){
    std::vector<double> vec(sim.n,0);
    std::vector<PriceContainer> paths(sim.M,PriceContainer(vec));
    std::normal_distribution<double> Z(0,1);
    double delta = static_cast<double>(sim.T)/static_cast<double>(sim.n);
    double multiplier=0;
    int last=(int)(s.close).size();

    for (int i=0; i<sim.M; i++){
        paths[i].close[0]=s.close[last-1];
    }

    for (int j=1; j<sim.n; j++){
        for (int i=0; i<sim.M; i++){
            multiplier = std::exp((p.r - 0.5*p.vola*p.vola)*delta + p.vola*std::sqrt(delta)*Z(rng));
            paths[i].close[j] = (paths[i].close[j-1]) * multiplier;
        }
    }
    return paths;
}

double mc_european_call_payoff(const Sim_Paramaeters& sim, PriceContainer& s, const Asset_Parameters &p, std::mt19937& rng){
    std::vector<PriceContainer> paths = simulateBGM_rn(sim, s, p, rng);
    double sum = 0.0;
    for (int i = 0; i < sim.M; ++i){
        sum += std::max(paths[i].close[sim.n-1] - p.k, 0.0);
    }
    double payoff_mean = (std::exp(-p.r*sim.T)) * (sum / static_cast<double>(sim.M));
    return payoff_mean;
}

double black_scholes(const Sim_Paramaeters &sim, PriceContainer& s, const Asset_Parameters& p){
    double d1= (std::log(s.current_price()/p.k)+(p.r+0.5*p.vola*p.vola)*sim.T)/(p.vola*std::sqrt(sim.T));
    double d2=d1-(p.vola*std::sqrt(sim.T));

    return s.current_price()*normalCDF(d1)-p.k*(std::exp(-p.r*sim.T))*normalCDF(d2);
}

std::vector<double> cholesky_GBM_r(const Sim_Paramaeters& sim, double w1, double w2, double rho, const Asset_Parameters& p1,const Asset_Parameters& p2, std::mt19937& rng){
    std::vector<double> vec(sim.M,0);
    std::normal_distribution<double> Z(0,1);
    double delta = static_cast<double>(sim.T)/static_cast<double>(sim.n);

    for (int i=0; i<sim.M; i++){
        double z1=Z(rng);
        double z2=Z(rng);
        double x1=z1;
        double x2=rho*z1+std::sqrt(1-rho*rho)*z2;

        double r1=p1.mu*delta+p1.vola*std::sqrt(delta)*x1;
        double r2=p2.mu*delta+p2.vola*std::sqrt(delta)*x2;

        vec[i]=w1*r1+w2*r2;
    }
    return vec;
}

double VaR(const Sim_Paramaeters& sim, std::vector<PriceContainer>& paths, double percentile){
    std::vector<double> vvar(sim.M,0);
    for (int i=0; i<sim.M; i++){
        vvar[i]=(paths[i].close[sim.n-1]);
    }
    std::sort(vvar.begin(),vvar.end());
    
    return vvar[sim.M*percentile]-paths[0].close[0];
}

