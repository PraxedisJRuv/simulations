#pragma once
#include "FABM_strat_functions.hpp"
#include <array>

struct StrategySimulationSummary {
    std::vector<double> prices;
    std::vector<std::array<int, 3>> active_buyers_by_type;
    std::vector<double> returns;
    std::vector<double> trader_wealths;
};

StrategySimulationSummary desired_action_test(std::vector<Trader>& traders, PriceContainer& price, double& fair_value,const double& lambda, const int& T){
    StrategySimulationSummary summary;

    summary.prices.push_back(price.current_price());

    for (int i=0; i<T; i++){
        std::array<int, 3> buyers{0, 0, 0};

        for (int j=0; j<3; j++){
            desired_action(traders[j], price, fair_value);
            if (traders[j].last_desicion == 1) {
                buyers[static_cast<std::size_t>(traders[j].strategy)]++;
            }
        }

        summary.active_buyers_by_type.push_back(buyers);
        update_price_and_wealth(traders, price, lambda);
        summary.prices.push_back(price.current_price());
    }

    for (const auto& trader : traders) {
        summary.trader_wealths.push_back(trader.wealth);
    }

    if (summary.prices.size() > 1) {
        for (std::size_t i = 1; i < summary.prices.size(); ++i) {
            summary.returns.push_back((summary.prices[i] / summary.prices[i - 1]) - 1.0);
        }
    }

    return summary;
}