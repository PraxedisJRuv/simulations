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

    PriceContainer returns(){
        std::vector<double> vreturns(close.size()-1);
        for (int i=0; i<close.size()-1; i++){
            vreturns[i]=(close[i+1]/close[i])-1;
        }
        return vreturns;
    }
};


std::vector<PriceContainer> simulateBGM(PriceContainer& s, double mu, double vola, int T, double M, double n){
    std::vector<double> vec(n+1,0);
    std::vector<PriceContainer> paths(M,PriceContainer(vec));
    std::mt19937 rng(777);
    std::normal_distribution<double> Z(0,1);
    double delta=T/n;
    double multiplier=0;
    int last=(s.close).size();

    for (double i=0; i<M; i++){
        paths[i].close[0]=s.close[last-1];
        std::cout<<paths[i].close[0]<<"\t";
    }
    std::cout<<"\n";

    for (double j=1; j<n; j++){

        for (double i=1; i<M; i++){
            multiplier =std::exp(mu*delta+vola*sqrt(delta)*Z(rng));
            paths[i].close[j]=((paths[i].close[j-1])*multiplier);
            std::cout<<paths[i].close[j]<<"\t";
        }
        std::cout<<"\n";
    }

    return paths;
}


std::mt19937 rng(777);

int main(){
    PriceContainer test({10,10});
    std::vector<PriceContainer> paths=simulateBGM(test,0.1,0.3,1,15,100);
    return 0;
}