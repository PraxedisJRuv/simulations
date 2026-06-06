#pragma once
#include "FABM_strat_strucutres.hpp"

void desired_action(Trader& t, const PriceContainer& p, const double fair_value){
    t.last_desicion=0;
    switch(t.strategy){

        case Strategy::Fundamentalist:{
            if(p.current_price()<fair_value*0.95){
                t.buy(p.current_price());
            }
            if(p.current_price()>fair_value*1.05){
                t.sell(p.current_price());
            }  
        }break;

        case Strategy::Chartist:{
            std::vector<double>returns=p.returns();
            int n=returns.size();
            bool flag=true;
            for (int i=n-5; i<n; i++){
                if (returns[i]<0){
                    flag=false;
                }
            }
            if(flag){
                t.buy(p.current_price());
            }
            else{
                t.sell(p.current_price());
            }
        }break;

        case Strategy::Random:{
            std::mt19937 rng(std::random_device{}());
            std::bernoulli_distribution coin_flip(0.5);
            if(coin_flip(rng)){
                t.buy(p.current_price());
            }
            else{
                t.sell(p.current_price());
            }
        }break;
    }
}


//lamba is liquidity, the smaller the lambda, the more liquid it is, the bigger the less operations it need to get afected
void update_price_and_wealth(std::vector<Trader>& traders, PriceContainer& p, double lambda){
    int buys=0;
    int sells=0;
    for (int i=0; i<traders.size(); i++){
        if(traders[i].last_desicion==1){
            buys++;
        }
        if(traders[i].last_desicion==-1){
            sells++;
        }
    }

    double price=0;
    double price_change=0;
    price = p.current_price()* std::exp(lambda*(buys-sells)/traders.size());
    price_change=price-p.current_price();
    p.update_price(price);
    
    for (int i=0; i<traders.size(); i++){
        traders[i].wealth=traders[i].wealth+traders[i].shares*price_change;
    }    
}