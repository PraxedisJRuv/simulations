#include "visualization.h"

int main(){
    double delta=0.5;
    int N = 200;
    int T = 200;
    double p = 0.04;
    Parameters param(0.2, 0.3, 0.4);
    double rate_E = n_E * param.sigma;
    double rate_I = n_I * param.gamma;

    std::mt19937 rng(std::random_device{}());
    std::bernoulli_distribution advance_E(1 - std::exp(-rate_E * delta));
    std::bernoulli_distribution advance_I(1 - std::exp(-rate_I * delta));

    std::vector<std::vector<int>> contacts = conctacts_list(N, p, rng);
    std::vector<Agent> agents;
    agents.reserve(N);
    
    for (int i = 0; i < N - 3; i++){
        agents.push_back({i, State::S, 0});
    }
    for (int i = N - 3; i < N; i++){
        agents.push_back({i, State::I1, 0});
    }

    runVisualization(delta,N,T,p,param,rng,advance_E,advance_I,contacts,agents);

    return 0;
}