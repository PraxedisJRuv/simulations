#include "pc_tests.hpp"
#include "pc_visualization.hpp"

int main(){
    PriceContainer price({100});
    Sim_Paramaeters sim(1,100,252);
    Asset_Parameters p(0.1,0.3,0.1,130);
    std::mt19937 rng(std::random_device{}());
    double percent=0.05;
    //test(sim, price, p, rng, 0.05);
    std::vector<PriceContainer> paths= graph_GBM_price_paths(sim, price, p, rng);
    std::pair<std::vector<double>, double> a=compute_pnl_and_var(paths, percent);
    graph_VaR(a.first,a.second);
    graph_GBM_animate_and_hist(paths);
    return 0;
}