#include<iostream>
#include<vector>
#include <cmath>
#include<random>

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

    PriceContainer returns(){
        std::vector<double> vreturns(close.size()-1);
        for (int i=0; i<close.size()-1; i++){
            vreturns[i]=(close[i+1]/close[i])-1;
        }
        return vreturns;
    }
};

double normalCDF(double x){
    return 0.5*std::erfc(-x/std::sqrt(2.0));
}

std::vector<PriceContainer> simulateBGM(PriceContainer& s, double mu, double vola, int T, double M, double n){
    std::vector<double> vec(n+1,0);
    std::vector<PriceContainer> paths(M,PriceContainer(vec));
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<double> Z(0,1);
    double delta=T/n;
    double multiplier=0;
    int last=(s.close).size();

    for (double i=0; i<M; i++){
        paths[i].close[0]=s.close[last-1];
        //std::cout<<paths[i].close[0]<<"\t";
    }
    //std::cout<<"\n";

    for (double j=1; j<n; j++){

        for (double i=1; i<M; i++){
            multiplier =std::exp(mu*delta+vola*sqrt(delta)*Z(rng));
            paths[i].close[j]=((paths[i].close[j-1])*multiplier);
            //std::cout<<paths[i].close[j]<<"\t";
        }
        //std::cout<<"\n";
    }

    return paths;
}

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

double mc_european_call_payoff(PriceContainer stock, double mu, double vola, int T, double M, double n, double k, double r){
    std::vector<PriceContainer> paths=simulateBGM(stock, mu, vola, T, M, n);
    double sum=0;
    for (int i=0; i<M; i++){
        sum=sum+std::max(paths[i].close[n-1]-k,0.0);
    }
    double payoff_mean=(std::exp(-r*T))*(sum/M);
    return payoff_mean;
}

double black_scholes(PriceContainer stock, double vola, int T, double k, double r){
    double d1= (std::log(stock.current_price()/k)+(r+0.5*vola*vola)*T)/(vola*std::sqrt(T));
    double d2=d1-(vola*std::sqrt(T));

    return stock.current_price()*normalCDF(d1)-k*(std::exp(-r*T))*normalCDF(d2);

}

int main(){
    PriceContainer test({10,10});
    //std::vector<PriceContainer> paths=simulateBGM(test,0.1,0.3,1,15,100);
    double mc_price=mc_european_call_payoff(test,0.1,0.3,1,100000,252,11.0,0.1);
    double bs_price=black_scholes(test,0.3,1,11.0,0.1);
    double error =std::abs(mc_price -bs_price);
    double errorpercent=error/bs_price;
    std::cout<<mc_price<<"\t"<<bs_price<<"\t"<<error<<"\t"<<errorpercent<<"%";
    return 0;
}