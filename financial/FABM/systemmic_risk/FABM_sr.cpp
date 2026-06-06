#include <iostream>
#include <random>
#include <vector>

enum class State{Solvent, Stressed, Defaulted};
struct Banks{
    int id;
    double capital;
    double total_assets;
    State current_state;
};

int N;
std::vector<std::vector<double>> exposure(N,std::vector<double>(N,0));
//define exposure function
//define contagion function

