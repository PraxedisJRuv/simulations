#include "FABM_strat_functions.hpp"

void desired_action_test(std::vector<Trader>& traders, PriceContainer& price, double& fair_value,const double& lambda, const int& T){
    
    for (int i=0; i<T; i++){
        for (int j=0; j<3; j++){
            desired_action(traders[j], price, fair_value);
        }
        update_price_and_wealth(traders, price, lambda);
                std::cout<<"price: "<<price.current_price()<<"\n";
    }
}