#include "pc_tests.hpp"

int main(){
    PriceContainer price({100});
    Sim_Paramaeters sim(1,10000,252);
    Asset_Parameters p(0.1,0.3,0.1,130);
    std::mt19937 rng(std::random_device{}());
    test(sim, price, p, rng, 0.05);

    return 0;
}