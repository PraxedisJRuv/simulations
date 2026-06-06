#pragma once
#include <iostream>
#include <random>
#include <algorithm>

enum class Strategy{Fundamentalist, Chartist, Random};

struct Trader{
    int id;
    double wealth;
    int shares;
    int last_desicion;
    Strategy strategy;

    Trader(int id, Strategy strategy){
        this->shares=0;
        this->id=id;
        this->strategy=strategy;
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<double>Z(500,1500);
        this->wealth=Z(rng);
        last_desicion=0;
    }

    void buy(double price){
        
        if(wealth>=price){
            wealth=wealth-price;
            shares=shares+1;
            last_desicion=1;
        }
    }

    void sell(double price){
        
        if(shares>=1){
            wealth=wealth+price;
            shares=shares-1;
            last_desicion=-1;
        }
    }
};

class PriceContainer{
    int size;
    public:
    
    std::vector<double> close;

    PriceContainer(std::vector<double> prices){
        close=prices;
        size=close.size()-1;
    }

    double mean()const{
        double sum=0;
        double cmean=0;
        for(int i=0; i<close.size(); i++){
            sum=sum+close[i];
        }
        cmean=sum/close.size();
        return cmean;
    }

    double std_dev()const{
        double cmean=mean();
        double desv=0;
        double sum =0;
        double aux=0;
        for (int i=0; i<close.size(); i++){
            aux=std::pow((close[i]-cmean),2);
            sum=sum+aux;
        }
        desv=std::sqrt((sum/(size)));
        return desv;
    }

    void print() const {
        for (int i=0;i<size; i++){
            std::cout<<close[i]<<", ";
        }
        std::cout<<"\n";
    }

    double current_price ()const{
        return close[size];
    }

    std::vector<double> returns()const{
        std::vector<double> vreturns((int)size);
        for (int i=0; i<(int)size; i++){
            vreturns[i]=(close[i+1]/close[i])-1.0;
        }
        return vreturns;
    }

    double max()const{
        std::vector<double> a=close;
        std::sort(a.begin(),a.end());
        return a[size];
    }

    double min()const{
     std::vector<double> a=close;
     std::sort(a.begin(),a.end());
     return a[0];   
    }

    void update_price(double new_price){
        close.push_back(new_price);
        size++;
    }
};