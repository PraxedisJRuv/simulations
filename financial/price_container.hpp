#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>


class PriceContainer{
    
    public:
    
    std::vector<double> close;

    PriceContainer(std::vector<double> prices){
        close=prices;
    }

    double mean(){
        double sum=0;
        double cmean=0;
        for(int i=0; i<close.size(); i++){
            sum=sum+close[i];
        }
        cmean=sum/close.size();
        return cmean;
    }

    double std_dev(){
        double cmean=mean();
        double desv=0;
        double sum =0;
        double aux=0;
        for (int i=0; i<close.size(); i++){
            aux=std::pow((close[i]-cmean),2);
            sum=sum+aux;
        }
        desv=std::sqrt((sum/(close.size()-1)));
        return desv;
    }

    void print(){
        for (int i=0;i<close.size(); i++){
            std::cout<<close[i]<<", ";
        }
        std::cout<<"\n";
    }

    double current_price(){
        int n=close.size();
        return close[n-1];
    }

    std::vector<double> returns(){
        std::vector<double> vreturns((int)close.size()-1);
        for (int i=0; i<(int)close.size()-1; i++){
            vreturns[i]=(close[i+1]/close[i])-1.0;
        }
        return vreturns;
    }

    double max(){
        std::vector<double> a=close;
        std::sort(a.begin(),a.end());
        return a[a.size()-1];
    }

    double min(){
     std::vector<double> a=close;
     std::sort(a.begin(),a.end());
     return a[0];   
    }
};

/*
mu is the expected rate of return. Percentage growth
vola is the volatility of the asset The standard deviation of the asset's continuously compounded (log) returns
T is the time horizon, generally is a year
M is the amount of paths
n is the amoun of steps in the simulation, that's why our "delta" is T/n
k is the strike or a fixed price for a stock with expiration T
r is the return risk-free rate
the exp(-r*T) is a discount thinking that x money tomorrow worths less than x money today
We're getting the current payoff for something sold at T
*/
struct Asset_Parameters{
    double mu, vola, r, k;
    Asset_Parameters(double mu, double vola, double r, double k){
        this->mu=mu;
        this->vola=vola;
        this->r=r;
        this->k=k;
    }
};

struct Sim_Paramaeters{
    int T, M, n;
    Sim_Paramaeters(int T, int M, int n){
        this->T=T;
        this->M=M;
        this->n=n;
    }
};