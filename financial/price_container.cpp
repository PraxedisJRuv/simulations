#include<iostream>
#include<vector>
#include <cmath>
#include<random>
#include <algorithm>

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

double flat_yield(PriceContainer stock, float t, double r){
    return stock.current_price()*std::exp(r*t);
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

std::vector<PriceContainer> simulateBGM_rn(PriceContainer& s, double vola, int T, double M, double n, double r){
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
            multiplier =std::exp((r-0.5*vola*vola)*delta+vola*sqrt(delta)*Z(rng));
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
    std::vector<PriceContainer> paths=simulateBGM_rn(stock, vola, T, M, n, r);
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

    std::vector<double> cholesky_GBM_r(double w1, double w2, double rho, double mu1, double mu2, double vola1, double vola2, double M, int T, double n){
    std::vector<double> vec(M,0);
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<double> Z(0,1);
    double delta=T/n;

    for (double i=0; i<M; i++){
        double z1=Z(rng);
        double z2=Z(rng);
        double x1=z1;
        double x2=rho*z1+std::sqrt(1-rho*rho)*z2;

        double r1=mu1*delta+vola1*std::sqrt(delta)*x1;
        double r2=mu2*delta+vola2*std::sqrt(delta)*x2;

        vec[i]=w1*r1+w2*r2;
    }
    return vec;
}

double VaR(std::vector<PriceContainer> paths, float percentile, int M, int n ){
    std::vector<double> vvar(M,0);
    for (int i=0; i<M; i++){
        vvar[i]=(paths[i].close[n-1]);
    }
    std::sort(vvar.begin(),vvar.end());
    
    return vvar[M*percentile]-paths[0].close[0];
}

int test(){
PriceContainer test({10,10});
    //std::vector<PriceContainer> paths=simulateBGM(test,0.1,0.3,1,15,100);
    double mc_price=mc_european_call_payoff(test,0.1,0.3,1,10000,252,11.0,0.1);
    double bs_price=black_scholes(test,0.3,1,11.0,0.1);
    double error =std::abs(mc_price -bs_price);
    double errorpercent=error/bs_price;
    std::cout<<mc_price<<"\t"<<bs_price<<"\t"<<error<<"\t"<<errorpercent<<"%";
    std::cout<<"\n"<<VaR(simulateBGM(test,0.1,0.3,1,100,252),0.01,100,252);
    return 0;
}

void cholesky_test(){
    int M=10000;
    std::vector <double> choleskyport=cholesky_GBM_r(0.6,0.4,1,0.07,0.03,0.20,0.05,M,1,252);
    std::sort(choleskyport.begin(),choleskyport.end());
    double var_1pct  = choleskyport[M * 0.01];
    double var_5pct  = choleskyport[M * 0.05];

    double mean = 0.0;
    
    for (double p : choleskyport) {
        mean = mean + p;
    }
    mean /= M;

    std::cout << "mean daily return : " << mean*100 << "%\n"
              << "1% VaR \t\t: " << var_1pct*100 << "%\n"
              << "5% VaR \t\t: " << var_5pct*100 << "%\n";

}

int main(){
    cholesky_test();
    return 0;
}