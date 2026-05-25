#include<iostream>
#include<vector>
#include <cmath>

class PriceContainer{
    std::vector<double> close;
    public:

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