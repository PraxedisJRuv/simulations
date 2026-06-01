#include<iostream>
#include<vector>
#include <cmath>
#include<random>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <limits>
#include <utility>

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

    std::vector<double> returns(){
        std::vector<double> vreturns((int)close.size()-1);
        for (int i=0; i<(int)close.size()-1; i++){
            vreturns[i]=(close[i+1]/close[i])-1.0;
        }
        return vreturns;
    }

    double max(){
        std::vector<double> a=close;
        std::sort(a.begin(),a.end());
        return a[a.size()-1];
    }

    double min(){
     std::vector<double> a=close;
     std::sort(a.begin(),a.end());
     return a[0];   
    }
};

double normalCDF(double x){
    return 0.5*std::erfc(-x/std::sqrt(2.0));
}

double flat_yield(PriceContainer stock, float t, double r){
    return stock.current_price()*std::exp(r*t);
}

std::vector<PriceContainer> simulateBGM(PriceContainer& s, double mu, double vola, int T, int M, int n){
    std::vector<double> vec(n,0);
    std::vector<PriceContainer> paths(M,PriceContainer(vec));
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<double> Z(0,1);
    double delta = static_cast<double>(T)/static_cast<double>(n);
    double multiplier=0;
    int last=(int)(s.close).size();

    for (int i=0; i<M; i++){
        paths[i].close[0]=s.close[last-1];
    }

    for (int j=1; j<n; j++){
        for (int i=0; i<M; i++){
            multiplier = std::exp(mu*delta + vola*std::sqrt(delta)*Z(rng));
            paths[i].close[j] = (paths[i].close[j-1]) * multiplier;
        }
    }

    return paths;
}

std::vector<PriceContainer> simulateBGM_rn(PriceContainer& s, double vola, int T, int M, int n, double r){
    std::vector<double> vec(n,0);
    std::vector<PriceContainer> paths(M,PriceContainer(vec));
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<double> Z(0,1);
    double delta = static_cast<double>(T)/static_cast<double>(n);
    double multiplier=0;
    int last=(int)(s.close).size();

    for (int i=0; i<M; i++){
        paths[i].close[0]=s.close[last-1];
    }

    for (int j=1; j<n; j++){
        for (int i=0; i<M; i++){
            multiplier = std::exp((r - 0.5*vola*vola)*delta + vola*std::sqrt(delta)*Z(rng));
            paths[i].close[j] = (paths[i].close[j-1]) * multiplier;
        }
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

double mc_european_call_payoff(PriceContainer stock, double mu, double vola, int T, int M, int n, double k, double r){
    std::vector<PriceContainer> paths = simulateBGM_rn(stock, vola, T, M, n, r);
    double sum = 0.0;
    for (int i = 0; i < M; ++i){
        sum += std::max(paths[i].close[n-1] - k, 0.0);
    }
    double payoff_mean = (std::exp(-r*T)) * (sum / static_cast<double>(M));
    return payoff_mean;
}

double black_scholes(PriceContainer stock, double vola, int T, double k, double r){
    double d1= (std::log(stock.current_price()/k)+(r+0.5*vola*vola)*T)/(vola*std::sqrt(T));
    double d2=d1-(vola*std::sqrt(T));

    return stock.current_price()*normalCDF(d1)-k*(std::exp(-r*T))*normalCDF(d2);

}

    std::vector<double> cholesky_GBM_r(double w1, double w2, double rho, double mu1, double mu2, double vola1, double vola2, int M, int T, int n){
    std::vector<double> vec(M,0);
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<double> Z(0,1);
    double delta = static_cast<double>(T)/static_cast<double>(n);

    for (int i=0; i<M; i++){
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

// forward-declare histogram drawer used by graph_GBM_price_paths
void graph_VaR(const std::vector<double>& pnl, double var_1pct);
// forward-declare animated drawer
void graph_GBM_animate_and_hist(const std::vector<PriceContainer>& paths);

// helper: compute P&L vector and 1% VaR from simulated paths
std::pair<std::vector<double>, double> compute_pnl_and_var(const std::vector<PriceContainer>& paths);

std::vector<PriceContainer> graph_GBM_price_paths(){
    // Simulation parameters
    PriceContainer start({100.0});
    double vola = 0.3;
    int T = 1;
    int M = 1000; // number of paths
    int n = 252; // steps
    double r = 0.03;

        std::vector<PriceContainer> paths = simulateBGM_rn(start, vola, T, M, n, r);

        // find min/max prices across all paths/time (for log scaling)
        double p_min = std::numeric_limits<double>::infinity();
        double p_max = -std::numeric_limits<double>::infinity();
        for (const auto &pc : paths){
            for (double p : pc.close){
                if (p < p_min) p_min = p;
                if (p > p_max) p_max = p;
            }
        }
        if (p_min <= 0) p_min = 1e-6;

        // SFML window
        const unsigned W = 1200, H = 700;
        const float pad = 60.0f;
        sf::RenderWindow window(sf::VideoMode(sf::Vector2u{W, H}), "GBM Price Paths (log scale)");
        window.setFramerateLimit(60);

        // Precompute log ranges and add a small margin so the fan isn't squashed
        double log_min = std::log(p_min);
        double log_max = std::log(p_max);
        double log_range = log_max - log_min;
        if (log_range <= 1e-6) {
            log_min -= 1.0;
            log_max += 1.0;
        } else {
            // increase margin so the fan isn't visually squashed
            double margin_factor = 0.12; // 12% margins
            log_min -= margin_factor * log_range;
            log_max += margin_factor * log_range;
        }

        // color for paths (low alpha to show fan density)
        sf::Color pathColor(55, 138, 221, 12);

        while (window.isOpen()){
            while (auto ev = window.pollEvent()){
                const sf::Event &e = *ev;
                if (e.is<sf::Event::Closed>()) window.close();
                else if (e.is<sf::Event::KeyPressed>()){
                    if (auto kp = e.getIf<sf::Event::KeyPressed>()){
                        if (kp->code == sf::Keyboard::Key::Escape) window.close();
                    }
                }
            }

            window.clear(sf::Color::White);

            // draw axes
            sf::VertexArray xAxis(sf::PrimitiveType::Lines, 2);
            xAxis[0].position = sf::Vector2f(pad, H - pad);
            xAxis[0].color = sf::Color(50,50,50);
            xAxis[1].position = sf::Vector2f(W - pad, H - pad);
            xAxis[1].color = sf::Color(50,50,50);
            sf::VertexArray yAxis(sf::PrimitiveType::Lines, 2);
            yAxis[0].position = sf::Vector2f(pad, pad);
            yAxis[0].color = sf::Color(50,50,50);
            yAxis[1].position = sf::Vector2f(pad, H - pad);
            yAxis[1].color = sf::Color(50,50,50);
            window.draw(xAxis);
            window.draw(yAxis);

            // draw each path - low alpha so dense regions darken
            for (const auto &pc : paths){
                sf::VertexArray va(sf::PrimitiveType::LineStrip, (size_t)n);
                for (int j = 0; j < n; ++j){
                    double price = pc.close[j];
                    double logp = std::log(price);
                    float x = pad + (float)j / (float)(n-1) * (W - 2*pad);
                        double t = (logp - log_min) / (log_max - log_min);
                        if (t < 0.0) t = 0.0; if (t > 1.0) t = 1.0;
                        float y = (float)((H - pad) - t * (H - 2*pad));
                    va[(size_t)j].position = sf::Vector2f(x,y);
                    va[(size_t)j].color = pathColor;
                }
                window.draw(va);
            }

            // draw initial price horizontal line
            double init_price = start.current_price();
            float init_y = (float)((H - pad) - (std::log(init_price) - log_min) / (log_max - log_min) * (H - 2*pad));
            sf::VertexArray initLine(sf::PrimitiveType::Lines, 2);
            initLine[0].position = sf::Vector2f(pad, init_y);
            initLine[0].color = sf::Color(150,150,150,200);
            initLine[1].position = sf::Vector2f(W - pad, init_y);
            initLine[1].color = sf::Color(150,150,150,200);
            window.draw(initLine);

            window.display();
        }

        // after closing the paths window, return simulated paths
        return paths;
    }

// compute P&L vector (percent returns) and 1% VaR from simulated paths
std::pair<std::vector<double>, double> compute_pnl_and_var(const std::vector<PriceContainer>& paths){
    std::pair<std::vector<double>, double> out;
    if (paths.empty()) return out;
    std::vector<double> pnl;
    pnl.reserve(paths.size());
    for (const auto &pc : paths){
        double start_p = pc.close.size() > 0 ? pc.close[0] : 0.0;
        // find last non-zero (or non-negligible) index in this path
        int last_idx = (int)pc.close.size() - 1;
        while (last_idx > 0 && pc.close[last_idx] <= 1e-12) --last_idx;
        double end_p = pc.close[last_idx];
        double ret_pct = 0.0;
        if (start_p > 0.0) ret_pct = (end_p / start_p - 1.0) * 100.0;
        pnl.push_back(ret_pct);
    }
    std::vector<double> sorted_pnl = pnl;
    std::sort(sorted_pnl.begin(), sorted_pnl.end());
    double var_1pct = 0.0;
    if (!sorted_pnl.empty()){
        int idx = std::max(0, (int)(sorted_pnl.size()*0.01));
        var_1pct = sorted_pnl[idx];
    }
    return {pnl, var_1pct};
}

    void graph_VaR(const std::vector<double>& pnl, double var_1pct){
        // histogram window
        const unsigned W = 1000, H = 600;
        const float pad = 60.0f;
        sf::RenderWindow window(sf::VideoMode(sf::Vector2u{W, H}), "P&L Histogram (VaR)");
        window.setFramerateLimit(60);

        // try to load a common Windows font for labels; if unavailable, skip text drawing
        sf::Font font;
        bool haveFont = false;
        try {
            haveFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");
        } catch (...) {
            haveFont = false;
        }

        int BINS = 80;
        double pnl_min = *std::min_element(pnl.begin(), pnl.end());
        double pnl_max = *std::max_element(pnl.begin(), pnl.end());
        if (pnl_max == pnl_min) pnl_max = pnl_min + 1e-6;
        double bin_width = (pnl_max - pnl_min) / BINS;

        std::vector<int> counts(BINS, 0);
        for (double p : pnl) {
            int b = 0;
            if (bin_width > 0.0) {
                b = (int)std::floor((p - pnl_min) / bin_width);
                if (b < 0) b = 0;
                if (b >= BINS) b = BINS - 1;
            }
            counts[b]++;
        }

        int max_count = *std::max_element(counts.begin(), counts.end());

        while (window.isOpen()){
            while (auto ev = window.pollEvent()){
                const sf::Event &e = *ev;
                if (e.is<sf::Event::Closed>()) window.close();
                else if (e.is<sf::Event::KeyPressed>()){
                    if (auto kp = e.getIf<sf::Event::KeyPressed>()){
                        if (kp->code == sf::Keyboard::Key::Escape) window.close();
                    }
                }
            }

            window.clear(sf::Color::White);

            float plot_width = W - 2*pad;
            float plot_height = H - 2*pad;

            for (int i = 0; i < BINS; ++i){
                float bar_w = plot_width / BINS - 2.0f;
                float bar_x = pad + (i / (float)BINS) * plot_width;
                float bar_h = (counts[i] / (float)max_count) * plot_height;
                float bar_y = pad + plot_height - bar_h;

                double bin_center = pnl_min + (i + 0.5) * bin_width;
                sf::Color col = bin_center < var_1pct
                              ? sf::Color(192, 76, 42, 200)
                              : sf::Color(55, 138, 221, 200);

                sf::RectangleShape bar(sf::Vector2f(bar_w, bar_h));
                bar.setPosition(sf::Vector2f(bar_x, bar_y));
                bar.setFillColor(col);
                window.draw(bar);
            }

            // VaR vertical line
            float var_x = pad + ((var_1pct - pnl_min) / (pnl_max - pnl_min)) * plot_width;
            sf::RectangleShape var_line(sf::Vector2f(2, plot_height));
            var_line.setPosition(sf::Vector2f(var_x, pad));
            var_line.setFillColor(sf::Color(192, 76, 42));
            window.draw(var_line);

            // Draw x-axis ticks and labels (5 ticks)
            if (haveFont) {
                sf::Text title(font, "P&L Histogram", 18);
                title.setFillColor(sf::Color::Black);
                title.setPosition(sf::Vector2f((float)(W/2) - 60.f, 8.f));
                window.draw(title);

                for (int t = 0; t <= 4; ++t) {
                    float tx = pad + (t / 4.0f) * plot_width;
                    // tick line
                    sf::VertexArray tick(sf::PrimitiveType::Lines, 2);
                    tick[0].position = sf::Vector2f(tx, H - pad);
                    tick[0].color = sf::Color::Black;
                    tick[1].position = sf::Vector2f(tx, H - pad + 6);
                    tick[1].color = sf::Color::Black;
                    window.draw(tick);

                    double val = pnl_min + (t / 4.0) * (pnl_max - pnl_min);
                    char buf[64];
                    std::snprintf(buf, sizeof(buf), "%.2f", val);
                    sf::Text lbl(font, buf, 12);
                    lbl.setFillColor(sf::Color::Black);
                    lbl.setPosition(sf::Vector2f(tx - 18.f, (float)H - pad + 8.f));
                    window.draw(lbl);
                }

                // y-axis label
                sf::Text ylbl(font, "Frequency", 12);
                ylbl.setFillColor(sf::Color::Black);
                ylbl.setPosition(sf::Vector2f(8.f, pad - 18.f));
                window.draw(ylbl);

                // VaR numeric annotation
                char vbuf[64];
                std::snprintf(vbuf, sizeof(vbuf), "VaR(1%%)=%.2f", var_1pct);
                sf::Text vt(font, vbuf, 14);
                vt.setFillColor(sf::Color(192,76,42));
                // place above the var line
                vt.setPosition(sf::Vector2f(var_x + 6.f, pad + 6.f));
                window.draw(vt);
            }

            window.display();
        }
    }

int main(){
    // Run the GBM price paths visualization, then animate histogram updating
    std::vector<PriceContainer> paths = graph_GBM_price_paths();
    graph_GBM_animate_and_hist(paths);
    
    return 0;
}

// Animate adding paths one-by-one and update histogram live
void graph_GBM_animate_and_hist(const std::vector<PriceContainer>& paths){
    if (paths.empty()) return;
    int M = (int)paths.size();
    int n = (int)paths[0].close.size();

    // precompute overall P&L range to fix histogram bins
    std::vector<double> all_pnl;
    all_pnl.reserve(M);
    for (const auto &pc : paths){
        double start_p = pc.close[0];
        double end_p = pc.close[n-1];
        double ret_pct = (end_p / start_p - 1.0) * 100.0;
        all_pnl.push_back(ret_pct);
    }
    double pnl_min = *std::min_element(all_pnl.begin(), all_pnl.end());
    double pnl_max = *std::max_element(all_pnl.begin(), all_pnl.end());
    if (pnl_max == pnl_min) pnl_max = pnl_min + 1e-6;

    const unsigned W1 = 1200, H1 = 700;
    const unsigned W2 = 1000, H2 = 600;
    const float pad = 60.f;

    sf::RenderWindow win_paths(sf::VideoMode(sf::Vector2u{W1,H1}), "GBM Paths (animated)");
    sf::RenderWindow win_hist(sf::VideoMode(sf::Vector2u{W2,H2}), "Histogram (live)");
    win_paths.setFramerateLimit(60);
    win_hist.setFramerateLimit(60);

    // histogram bins
    int BINS = 80;
    double bin_width = (pnl_max - pnl_min) / BINS;
    std::vector<int> counts(BINS, 0);
    std::vector<double> collected_pnl;
    collected_pnl.reserve(M);

    // path drawing params
    sf::Color pathColor(55,138,221,20);

    // precompute log scaling for path window
    double p_min = std::numeric_limits<double>::infinity();
    double p_max = -std::numeric_limits<double>::infinity();
    for (const auto &pc : paths){ for (double p : pc.close){ if (p < p_min) p_min = p; if (p > p_max) p_max = p; }}
    if (p_min <= 0) p_min = 1e-6;
    double log_min = std::log(p_min);
    double log_max = std::log(p_max);
    double log_range = log_max - log_min;
    if (log_range <= 1e-6){ log_min -= 1.0; log_max += 1.0; }
    else { double margin = 0.12; log_min -= margin*log_range; log_max += margin*log_range; }

    size_t visible = 0;
    sf::Font font;
    bool haveFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");

    while (win_paths.isOpen() || win_hist.isOpen()){
        // handle events for both windows
        while (auto ev = win_paths.pollEvent()){
            const sf::Event &e = *ev;
            if (e.is<sf::Event::Closed>()) win_paths.close();
            else if (e.is<sf::Event::KeyPressed>()){
                if (auto kp = e.getIf<sf::Event::KeyPressed>()) if (kp->code == sf::Keyboard::Key::Escape) win_paths.close();
            }
        }
        while (auto ev = win_hist.pollEvent()){
            const sf::Event &e = *ev;
            if (e.is<sf::Event::Closed>()) win_hist.close();
            else if (e.is<sf::Event::KeyPressed>()){
                if (auto kp = e.getIf<sf::Event::KeyPressed>()) if (kp->code == sf::Keyboard::Key::Escape) win_hist.close();
            }
        }

        // increment visible paths (faster reveal)
        if (visible < paths.size()) visible += 3;

        // if a new path became visible, add its P&L to histogram
        if (!collected_pnl.empty() || visible>0) {
            size_t prev = collected_pnl.size();
            for (size_t i = prev; i < visible && i < paths.size(); ++i){
                double start_p = paths[i].close[0];
                double end_p = paths[i].close[n-1];
                double ret = (end_p/start_p - 1.0) * 100.0;
                collected_pnl.push_back(ret);
                int b = 0;
                if (bin_width > 0.0) {
                    b = (int)std::floor((ret - pnl_min) / bin_width);
                    if (b < 0) b = 0;
                    if (b >= BINS) b = BINS - 1;
                }
                counts[b]++;
            }
        }

        // draw paths window
        if (win_paths.isOpen()){
            win_paths.clear(sf::Color::White);

            // axes
            sf::VertexArray xAxis(sf::PrimitiveType::Lines,2);
            xAxis[0].position = sf::Vector2f(pad, H1 - pad); xAxis[0].color = sf::Color(50,50,50);
            xAxis[1].position = sf::Vector2f(W1 - pad, H1 - pad); xAxis[1].color = sf::Color(50,50,50);
            sf::VertexArray yAxis(sf::PrimitiveType::Lines,2);
            yAxis[0].position = sf::Vector2f(pad, pad); yAxis[0].color = sf::Color(50,50,50);
            yAxis[1].position = sf::Vector2f(pad, H1 - pad); yAxis[1].color = sf::Color(50,50,50);
            win_paths.draw(xAxis); win_paths.draw(yAxis);

            // draw visible paths (thick lines as rectangles)
            for (size_t i = 0; i < visible && i < paths.size(); ++i){
                const auto &pc = paths[i];
                for (int j=0;j<n-1;++j){
                    double price1 = pc.close[j];
                    double logp1 = std::log(price1);
                    float x1 = pad + (float)j / (float)(n-1) * (W1 - 2*pad);
                    double t1 = (logp1 - log_min) / (log_max - log_min);
                    if (t1 < 0.0) t1 = 0.0; if (t1 > 1.0) t1 = 1.0;
                    float y1 = (float)((H1 - pad) - t1 * (H1 - 2*pad));
                    
                    double price2 = pc.close[j+1];
                    double logp2 = std::log(price2);
                    float x2 = pad + (float)(j+1) / (float)(n-1) * (W1 - 2*pad);
                    double t2 = (logp2 - log_min) / (log_max - log_min);
                    if (t2 < 0.0) t2 = 0.0; if (t2 > 1.0) t2 = 1.0;
                    float y2 = (float)((H1 - pad) - t2 * (H1 - 2*pad));
                    
                    // draw thick line segment as rectangle
                    float dx = x2 - x1, dy = y2 - y1;
                    float len = std::sqrt(dx*dx + dy*dy);
                    if (len > 0.01f) {
                        float thick = 2.5f;
                        float nx = -dy / len * thick, ny = dx / len * thick;
                        sf::VertexArray seg(sf::PrimitiveType::TriangleStrip, 4);
                        seg[0].position = sf::Vector2f(x1 - nx, y1 - ny); seg[0].color = pathColor;
                        seg[1].position = sf::Vector2f(x1 + nx, y1 + ny); seg[1].color = pathColor;
                        seg[2].position = sf::Vector2f(x2 - nx, y2 - ny); seg[2].color = pathColor;
                        seg[3].position = sf::Vector2f(x2 + nx, y2 + ny); seg[3].color = pathColor;
                        win_paths.draw(seg);
                    }
                }
            }
            
            // draw percentile overlays (5th, 50th, 95th)
            std::vector<std::vector<double>> percentiles_by_time(n);
            for (size_t i = 0; i < visible && i < paths.size(); ++i){
                for (int j = 0; j < n; ++j){
                    percentiles_by_time[j].push_back(std::log(paths[i].close[j]));
                }
            }
            for (int j = 0; j < n; ++j){
                if (!percentiles_by_time[j].empty()){
                    std::sort(percentiles_by_time[j].begin(), percentiles_by_time[j].end());
                    double p5 = percentiles_by_time[j][(int)(percentiles_by_time[j].size() * 0.05)];
                    double p50 = percentiles_by_time[j][(int)(percentiles_by_time[j].size() * 0.50)];
                    double p95 = percentiles_by_time[j][(int)(percentiles_by_time[j].size() * 0.95)];
                    
                    auto draw_percentile_point = [&](double logp, sf::Color col) {
                        double t = (logp - log_min) / (log_max - log_min);
                        if (t < 0.0) t = 0.0; if (t > 1.0) t = 1.0;
                        float x = pad + (float)j / (float)(n-1) * (W1 - 2*pad);
                        float y = (float)((H1 - pad) - t * (H1 - 2*pad));
                        sf::CircleShape dot(1.5f);
                        dot.setPosition(sf::Vector2f(x - 1.5f, y - 1.5f));
                        dot.setFillColor(col);
                        win_paths.draw(dot);
                    };
                    draw_percentile_point(p5, sf::Color(100,100,100,180));
                    draw_percentile_point(p50, sf::Color(0,200,0,220));
                    draw_percentile_point(p95, sf::Color(100,100,100,180));
                }
            }

            win_paths.display();
        }

        // draw histogram window
        if (win_hist.isOpen()){
            win_hist.clear(sf::Color::White);
            float plot_width = W2 - 2*pad;
            float plot_height = H2 - 2*pad;
            
            // compute dynamic VaR from collected paths (recompute here for coloring)
            double var = 0.0;
            if (!collected_pnl.empty()){
                auto tmp = collected_pnl; std::sort(tmp.begin(), tmp.end());
                int idx = std::max(0, (int)(tmp.size()*0.01));
                var = tmp[idx];
            }
            
            int max_count = collected_pnl.empty() ? 1 : *std::max_element(counts.begin(), counts.end());
            for (int i=0;i<BINS;++i){
                float bar_w = plot_width / BINS - 2.0f;
                float bar_x = pad + (i / (float)BINS) * plot_width;
                float bar_h = (counts[i] / (float)std::max(1, max_count)) * plot_height;
                float bar_y = pad + plot_height - bar_h;
                double bin_center = pnl_min + (i + 0.5) * bin_width;
                sf::Color col = bin_center < var ? sf::Color(192,76,42,200) : sf::Color(55,138,221,200);
                sf::RectangleShape bar(sf::Vector2f(bar_w, bar_h));
                bar.setPosition(sf::Vector2f(bar_x, bar_y));
                bar.setFillColor(col);
                win_hist.draw(bar);
            }

            // VaR vertical line
            float var_x = pad + ((var - pnl_min) / (pnl_max - pnl_min)) * plot_width;
            sf::RectangleShape var_line(sf::Vector2f(2, plot_height));
            var_line.setPosition(sf::Vector2f(var_x, pad));
            var_line.setFillColor(sf::Color(192,76,42));
            win_hist.draw(var_line);

            if (haveFont){
                sf::Text title(font, "P&L Histogram (live)", 16);
                title.setFillColor(sf::Color::Black);
                title.setPosition(sf::Vector2f((float)(W2/2) - 80.f, 8.f));
                win_hist.draw(title);
                char vbuf[64]; std::snprintf(vbuf, sizeof(vbuf), "VaR(1%%)=%.2f", var);
                sf::Text vt(font, vbuf, 14); vt.setFillColor(sf::Color(192,76,42)); vt.setPosition(sf::Vector2f(var_x + 6.f, pad + 6.f)); win_hist.draw(vt);
            }

            win_hist.display();
        }

        // small sleep to control frame timing
        sf::sleep(sf::milliseconds(10));

        // exit when both windows closed or all paths visible and user closed
        if (!win_paths.isOpen() && !win_hist.isOpen()) break;
        if (visible >= paths.size() && !win_paths.isOpen()) { /* allow histogram to remain */ }
    }
}