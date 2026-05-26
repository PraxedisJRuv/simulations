#include<iostream>
#include<cmath>
#include<numbers>

float e_manual_f(double n, float r, int t){
    float em=(1+(r/n));
    return std::pow(em,n*t);
}

double e_manual_d(double n, float r, int t){
    double em=(1+(r/n));
    return std::pow(em,n*t);
}

long double e_manual_ld(double n, float r, int t){
    long double em=(1+(r/n));
    return std::pow(em,n*t);
}

int main(){
    float r=0.5;
    int t=10;
    
    std::cout<<"n \t"<<"float \t"<<"double \t"<<"long double \t"<<"exponential \n";
    for (double i=1; i<=10000000; i=i*10){
        std::cout<<i<<"\t"
                 <<e_manual_f(i,r,t)<<"\t"
                 <<e_manual_d(i,r,t)<<"\t"
                 <<e_manual_ld(i,r,t)<<"\t\t"
                 <<std::exp(r*t)<<"\n";
    }

    return 0;
}