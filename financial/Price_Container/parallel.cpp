#include<iostream>
#include<cmath>
#include<random>
#include<vector>
#include<chrono>
#include<omp.h>

double normalCDF(double x){
    return 0.5*std::erfc(-x/std::sqrt(2.0));
}

double blackScholesCall(double S0, double K, double r, double sigma, double T){
    double d1 = (std::log(S0/K) + (r+0.5*sigma*sigma)*T)/(sigma*std::sqrt(T));
    double d2 = d1 - (sigma*std::sqrt(T));
    return S0*normalCDF(d1)-K*(std::exp(-r*T))*normalCDF(d2);
}

double monteCarloPricer(double S0, double K, double r,
                        double sigma, double T,
                        int M, int steps, int base_seed) {
    double dt      = T / steps;
    double drift   = (r - 0.5*sigma*sigma) * dt;
    double diffusion = sigma * std::sqrt(dt);

    double sum = 0.0;

    #pragma omp parallel reduction(+:sum) //here is when the actual omp implementation begins
    {
        int tid = omp_get_thread_num();
        std::mt19937 rng(base_seed + tid * 1000003); // large prime offset
        std::normal_distribution<double> Z(0.0, 1.0);

        #pragma omp for schedule(dynamic, 512)
        for (int path = 0; path < M; path++) {
            double S = S0;
            for (int t = 0; t < steps; t++) {
                S *= std::exp(drift + diffusion * Z(rng));
            }
            double payoff = std::max(S - K, 0.0);
            sum += payoff;
        }
    }

    return std::exp(-r*T) * sum / M;
}

using Clock = std::chrono::high_resolution_clock;
using Ms    = std::chrono::duration<double, std::milli>;


int main() {
    // option parameters
    const double S0    = 100.0;
    const double K     = 100.0;
    const double r     = 0.05;
    const double sigma = 0.20;
    const double T     = 1.0;
    const int    steps = 252;
    const int    M     = 1'000'000;

    double bs_price = blackScholesCall(S0, K, r, sigma, T);
    std::cout << "Black-Scholes price: " << bs_price << "\n\n";

    // sweep thread counts: 1, 2, 4, 8
    std::vector<int> thread_counts = {1, 2, 4, 8};

    double time_sequential = 0.0;

    for (int n_threads : thread_counts) {
        omp_set_num_threads(n_threads);

        auto t0 = Clock::now();
        double mc_price = monteCarloPricer(S0, K, r, sigma, T,
                                           M, steps, 42);
        double elapsed = Ms(Clock::now() - t0).count();

        if (n_threads == 1) time_sequential = elapsed;
        double speedup = time_sequential / elapsed;

        std::cout << "threads=" << n_threads
                  << "  price="   << mc_price
                  << "  error="   << std::abs(mc_price - bs_price)
                  << "  time="    << elapsed    << " ms"
                  << "  speedup=" << speedup    << "x\n";
    }

    return 0;
}