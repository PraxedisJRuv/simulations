#include <iostream>
#include <random>
#include <algorithm>

enum class Strategy{Fundamentalist, Chartist, Random};

struct Trader{
    int id;
    double wealth;
    int shares;
    int last_desicion;
    Strategy strategy;

    Trader(int id, Strategy strategy){
        this->shares=0;
        this->id=id;
        this->strategy=strategy;
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<double>Z(500,1500);
        this->wealth=Z(rng);
        last_desicion=0;
    }

    void buy(double price){
        
        if(wealth>=price){
            wealth=wealth-price;
            shares=shares+1;
            last_desicion=1;
        }
    }

    void sell(double price){
        
        if(shares>=1){
            wealth=wealth+price;
            shares=shares-1;
            last_desicion=-1;
        }
    }
};

class PriceContainer{
    int size;
    public:
    
    std::vector<double> close;

    PriceContainer(std::vector<double> prices){
        close=prices;
        size=close.size()-1;
    }

    double mean()const{
        double sum=0;
        double cmean=0;
        for(int i=0; i<close.size(); i++){
            sum=sum+close[i];
        }
        cmean=sum/close.size();
        return cmean;
    }

    double std_dev()const{
        double cmean=mean();
        double desv=0;
        double sum =0;
        double aux=0;
        for (int i=0; i<close.size(); i++){
            aux=std::pow((close[i]-cmean),2);
            sum=sum+aux;
        }
        desv=std::sqrt((sum/(size)));
        return desv;
    }

    void print() const {
        for (int i=0;i<size; i++){
            std::cout<<close[i]<<", ";
        }
        std::cout<<"\n";
    }

    double current_price ()const{
        return close[size];
    }

    std::vector<double> returns()const{
        std::vector<double> vreturns((int)size);
        for (int i=0; i<(int)size; i++){
            vreturns[i]=(close[i+1]/close[i])-1.0;
        }
        return vreturns;
    }

    double max()const{
        std::vector<double> a=close;
        std::sort(a.begin(),a.end());
        return a[size];
    }

    double min()const{
     std::vector<double> a=close;
     std::sort(a.begin(),a.end());
     return a[0];   
    }

    void update_price(double new_price){
        close.push_back(new_price);
        size++;
    }
};

void desired_action(Trader& t, const PriceContainer& p, const double fair_value){
    t.last_desicion=0;
    switch(t.strategy){

        case Strategy::Fundamentalist:{
            if(p.current_price()<fair_value*0.95){
                t.buy(p.current_price());
            }
            if(p.current_price()>fair_value*1.05){
                t.sell(p.current_price());
            }  
        }break;

        case Strategy::Chartist:{
            std::vector<double>returns=p.returns();
            int n=returns.size();
            bool flag=true;
            for (int i=n-5; i<n; i++){
                if (returns[i]<0){
                    flag=false;
                }
            }
            if(flag){
                t.buy(p.current_price());
            }
            else{
                t.sell(p.current_price());
            }
        }break;

        case Strategy::Random:{
            std::mt19937 rng(std::random_device{}());
            std::bernoulli_distribution coin_flip(0.5);
            if(coin_flip(rng)){
                std::cout<<"entro la moneda";
                t.buy(p.current_price());
            }
            else{
                t.sell(p.current_price());
            }
        }break;
    }
}


//lamba is liquidity, the smaller the lambda, the more liquid it is, the bigger the less operations it need to get afected
void update_price_and_wealth(std::vector<Trader>& traders, PriceContainer& p, double lambda){
    int buys=0;
    int sells=0;
    for (int i=0; i<traders.size(); i++){
        if(traders[i].last_desicion==1){
            buys++;
        }
        if(traders[i].last_desicion==-1){
            sells++;
        }
    }

    double price=0;
    double price_change=0;
    price = p.current_price()* std::exp(lambda*(buys-sells)/traders.size());
    price_change=price-p.current_price();
    p.update_price(price);
    
    for (int i=0; i<traders.size(); i++){
        traders[i].wealth=traders[i].wealth+traders[i].shares*price_change;
    }    
}

void desired_action_test(){
    
    Trader a(1,Strategy::Fundamentalist);
    Trader b(1,Strategy::Chartist);
    Trader c(1,Strategy::Random);

    std::vector<Trader> traders={a,b,c};
    PriceContainer price({10,11,11,12,13,13.2,13.7,14.1});

    double fair_value=13.5;

    for (int i=0; i<10; i++){
        for (int j=0; j<3; j++){
            desired_action(traders[j], price, fair_value);
        }
        update_price_and_wealth(traders, price, 0.8);
        std::cout<<"a: "<<traders[0].shares<<"\t"<<traders[0].wealth<<"\n"
                <<"b: "<<traders[1].shares<<"\t"<<traders[1].wealth<<"\n"
                <<"c: "<<traders[2].shares<<"\t"<<traders[2].wealth<<"\n"
                <<"price: "<<price.current_price()<<"\n";
    }
}


/*
Excersises to do wer to test the effects on markets with different proportions regarding strategies
in sizes greater than 200
Note the top % wealths and the opposite, the % with less wealth after certain time and the strategies 
proportion
*/

int main(){
    desired_action_test();

    return 0;
}