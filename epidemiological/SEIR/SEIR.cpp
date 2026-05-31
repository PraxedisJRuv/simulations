#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <SFML/Graphics.hpp>
// Window and drawing constants
const unsigned int W = 900;
const unsigned int H = 600;
const float LEFT_MARGIN = 50.f;
const float RIGHT_MARGIN = 50.f;
const float TOP_MARGIN = 50.f;
const float BOTTOM_MARGIN = 50.f;

// Convert simulation (time, value) to screen coordinates
static sf::Vector2f toScreen(double t, double value, double T_max, double N) {
    float x = LEFT_MARGIN + static_cast<float>(t / T_max) * (W - LEFT_MARGIN - RIGHT_MARGIN);
    // invert y because screen y grows downwards
    float y = (H - BOTTOM_MARGIN) - static_cast<float>(value / N) * (H - TOP_MARGIN - BOTTOM_MARGIN);
    return sf::Vector2f(x, y);
}

static void drawThickCurve(sf::RenderTarget& target, const std::vector<double>& times, const std::vector<double>& values, double T_max, double N, const sf::Color& color, float thickness) {
    if (times.size() < 2 || times.size() != values.size())
        return;

    for (size_t i = 1; i < times.size(); ++i) {
        sf::Vector2f p0 = toScreen(times[i - 1], values[i - 1], T_max, N);
        sf::Vector2f p1 = toScreen(times[i], values[i], T_max, N);
        sf::Vector2f delta = p1 - p0;
        float length = std::hypot(delta.x, delta.y);
        if (length <= 0.0f)
            continue;

        sf::RectangleShape segment(sf::Vector2f(length, thickness));
        segment.setFillColor(color);
        segment.setOrigin(sf::Vector2f(0.f, thickness / 2.f));
        segment.setPosition(p0);
        segment.setRotation(sf::degrees(std::atan2(delta.y, delta.x) * 180.f / 3.14159265f));
        target.draw(segment);
    }
}
/*
S is suceptible
E is Exposed
I is Infected
R is Recovered

beta is the infection rate for S regarding I
gamma is the recovery rate for I
sigma is the infection rate for E
vax is the vacciantion rate for S

*/

struct Population{
    double S, E, I, R, N;

    Population(double s, double e, double i, double r){
        S=s; 
        E=e; 
        I=i; 
        R=r;
        N=s+e+i+r;
    }

    friend std::ostream& operator<<(std::ostream& os, const Population& p){
        os << "|S =" << p.S
           << " | E ="<<p.E
           << " | I =" << p.I
           << " | R =" << p.R
           << "\n";

        return os;
    }
    
    Population det_deriv(Population& p, float beta, float gamma, float sigma, float vax){
            double new_S_infections=(beta*p.S*p.I)/p.N;
            double new_E_infections=(sigma*p.E);
            double new_recoveries=(gamma*p.I);
            double new_vaccinated=(vax*p.S);

            return{
                -new_S_infections-new_vaccinated,
                new_S_infections-new_E_infections,
                new_E_infections-new_recoveries,
                new_recoveries + new_vaccinated
            };

    }

    Population change(double T_end, double delta, Population& p, float beta, float gamma, float sigma, float vax){
        double t=0;
        std::cout<<std::fixed<<std::setprecision(4);
        while(t<T_end){
            Population slope=det_deriv(p, beta, gamma, sigma, vax);
            p.S=p.S+delta*slope.S;
            p.E=p.E+delta*slope.E;
            p.I=p.I+delta*slope.I;
            p.R=p.R+delta*slope.R;
            
            t=t+delta;
            std::cout<<"At t ="<<t<<"\t";
            std::cout << p; 
            }
        std::cout << "Approximate solution at time = " << T_end << " is " << p << "\n";
        return {p.S, p.E, p.I, p.R};
        
    }

    Population rnd_change(double T_end, double delta, Population& s, float beta, float gamma, float sigma, float vax){
        std::mt19937 rng(std::random_device{}());

        double t=0;
        while(t<T_end){
            std::binomial_distribution<int> binom_SR(s.S,1-std::exp((-vax*delta)));
            int new_vaxxed=binom_SR(rng);
            
            std::binomial_distribution<int> binom_SE(s.S-new_vaxxed,1-std::exp((-beta*s.I*delta)/(s.N)));
            std::binomial_distribution<int> binom_EI(s.E,1-std::exp((-sigma*delta)));
            std::binomial_distribution<int> binom_IR(s.I,1-std::exp((-gamma*delta)));
            
            int new_exposed=binom_SE(rng);
            int new_infected=binom_EI(rng);
            int new_recovered=binom_IR(rng);
            

            s.S=s.S-new_exposed-new_vaxxed;
            s.E=s.E+new_exposed-new_infected;
            s.I=s.I+new_infected-new_recovered;
            s.R=s.R+new_recovered+new_vaxxed;

            t=t+delta;
            std::cout<<"At t ="<<t<<"\t";
            std::cout <<s<<"\n"; 
        }
        std::cout << "Approximate solution at time = " << T_end << " is " << s << "\n";
        return {s.S, s.E, s.I, s.R};
        
    }

    std::vector<double> get_sim_peaks(int M, Population& s, double T_end, double delta, float beta, float gamma, float sigma, float vax){
        std::mt19937 rng(std::random_device{}());
        double peak =0;
        double t=0;
        double N=s.S+s.I+s.R;
        std::vector<double> peaks(M,0);
        
        for (int i=0; i<M; i++){
            s.S=50;
            s.I=2;
            s.R=1;

            peak=0;
            t=0;
        while(t<T_end){

            std::binomial_distribution<int> binom_SR(s.S,1-std::exp((-vax*delta)));
            int new_vaxxed=binom_SR(rng);
            //the previous had to be done since both probabilities afre independent in the same population.
            std::binomial_distribution<int> binom_SE(s.S-new_vaxxed,1-std::exp((-beta*s.I*delta)/(s.N)));
            std::binomial_distribution<int> binom_EI(s.E,1-std::exp((-sigma*delta)));
            std::binomial_distribution<int> binom_IR(s.I,1-std::exp((-gamma*delta)));
            
            int new_exposed=binom_SE(rng);
            int new_infected=binom_EI(rng);
            int new_recovered=binom_IR(rng);
            

            s.S=s.S-new_exposed-new_vaxxed;
            s.E=s.E+new_exposed-new_infected;
            s.I=s.I+new_infected-new_recovered;
            s.R=s.R+new_recovered+new_vaxxed;

            t=t+delta;
            peak=std::max(peak,s.I);
        }
        peaks[i]=peak;
        }
        
        return peaks;
    }

    
           
};

void print_peaks_p(std::vector<double>peaks){
        std::sort(peaks.begin(),peaks.end());
    int p5  = peaks[peaks.size()-1 * 0.05];
    int p50 = peaks[peaks.size()-1 * 0.50];
    int p95 = peaks[peaks.size()-1 * 0.95];
    std::cout<<"\n"<<peaks[p5]<<"\t"<<peaks[p50]<<"\t"<<peaks[p95]<<"\t"<<peaks[peaks.size()-1];
}

void test_peaks(){
    //std::cout<<pobla2;
    //pobla2.rnd_change(20,0.5, pobla2, 0.8, 0.3);

    
    Population pobla(50,4,2,1);
    pobla.change(10,.05,pobla,0.5,0.3,0.9,0.01);

    pobla.S=50;
    pobla.E=4;
    pobla.I=2;
    pobla.R=1;

    pobla.rnd_change(10,.05,pobla,0.5,0.3,0.9,0.01);


    Population pobla2(50,4,2,1);
    std::vector<double> peaks=pobla2.get_sim_peaks(100,pobla2,10,0.5,0.5,0.3,.9,.01);
    print_peaks_p(peaks);
}


int main(){
    sf::RenderWindow window(sf::VideoMode({900, 600}), "SEIR Simulation");
    Population pobla(1000,4,2,1);
    std::vector<Population> results;

    // start with the initial state at t = 0
    results.push_back(pobla);
    const int numSteps = 1000;
    for (int i = 0; i < numSteps; ++i) {
        results.push_back(pobla.rnd_change(0.5, 0.5, pobla, 0.4, 0.01, 0.4, 0.001));
    }

    std::vector<double> times, S_curve, E_curve, I_curve, R_curve;
    times.reserve(results.size());
    S_curve.reserve(results.size());
    E_curve.reserve(results.size());
    I_curve.reserve(results.size());
    R_curve.reserve(results.size());

    for (size_t i = 0; i < results.size(); ++i) {
        times.push_back(i * 0.5);
        S_curve.push_back(results[i].S);
        E_curve.push_back(results[i].E);
        I_curve.push_back(results[i].I);
        R_curve.push_back(results[i].R);
    }
    
    // compute plot scaling
    double T_max = (times.empty() ? 1.0 : times.back());
    double N_total = 1.0;
    if (!results.empty()) {
        N_total = results[0].S + results[0].E + results[0].I + results[0].R;
        if (N_total <= 0) N_total = 1.0;
    }

    sf::Font font;
    bool fontLoaded = font.openFromFile("arial.ttf");

    while (window.isOpen()) {
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::White);

        drawThickCurve(window, times, S_curve, T_max, N_total, sf::Color::Blue, 3.f);
        drawThickCurve(window, times, E_curve, T_max, N_total, sf::Color(255, 191, 0), 3.f);
        drawThickCurve(window, times, I_curve, T_max, N_total, sf::Color(192, 76, 42), 3.f);
        drawThickCurve(window, times, R_curve, T_max, N_total, sf::Color(0, 128, 128), 3.f);

        // X axis
        sf::RectangleShape xAxis(sf::Vector2f(W - LEFT_MARGIN - RIGHT_MARGIN, 1));
        xAxis.setPosition(sf::Vector2f(LEFT_MARGIN, static_cast<float>(H) - BOTTOM_MARGIN));
        xAxis.setFillColor(sf::Color::Black);
        window.draw(xAxis);

        // Y axis
        sf::RectangleShape yAxis(sf::Vector2f(1, H - TOP_MARGIN - BOTTOM_MARGIN));
        yAxis.setPosition(sf::Vector2f(LEFT_MARGIN, TOP_MARGIN));
        yAxis.setFillColor(sf::Color::Black);
        window.draw(yAxis);

        // Axis ticks and labels
        const int xTicks = 5;
        const int yTicks = 5;
        for (int i = 0; i < xTicks; ++i) {
            float t = static_cast<float>(i) / (xTicks - 1);
            float x = LEFT_MARGIN + t * (W - LEFT_MARGIN - RIGHT_MARGIN);
            sf::RectangleShape tick(sf::Vector2f(1, 8));
            tick.setPosition(sf::Vector2f(x, static_cast<float>(H) - BOTTOM_MARGIN - 4.f));
            tick.setFillColor(sf::Color::Black);
            window.draw(tick);

            if (fontLoaded) {
                std::ostringstream os;
                os << std::fixed << std::setprecision(1) << (T_max * t);
                sf::Text label(font, os.str(), 12u);
                label.setFillColor(sf::Color::Black);
                sf::FloatRect bounds = label.getLocalBounds();
                label.setOrigin(sf::Vector2f(bounds.size.x / 2.f, -2.f));
                label.setPosition(sf::Vector2f(x, static_cast<float>(H) - BOTTOM_MARGIN + 4.f));
                window.draw(label);
            }
        }
        for (int i = 0; i < yTicks; ++i) {
            float v = static_cast<float>(i) / (yTicks - 1);
            float y = (H - BOTTOM_MARGIN) - v * (H - TOP_MARGIN - BOTTOM_MARGIN);
            sf::RectangleShape tick(sf::Vector2f(8, 1));
            tick.setPosition(sf::Vector2f(LEFT_MARGIN - 4.f, y));
            tick.setFillColor(sf::Color::Black);
            window.draw(tick);

            if (fontLoaded) {
                std::ostringstream os;
                os << std::fixed << std::setprecision(0) << (N_total * v);
                sf::Text label(font, os.str(), 12u);
                label.setFillColor(sf::Color::Black);
                sf::FloatRect bounds = label.getLocalBounds();
                label.setOrigin(sf::Vector2f(bounds.size.x + 4.f, bounds.size.y / 2.f));
                label.setPosition(sf::Vector2f(LEFT_MARGIN - 8.f, y));
                window.draw(label);
            }
        }

        if (fontLoaded) {
            sf::Text xLabel(font, "Days", 14u);
            xLabel.setFillColor(sf::Color::Black);
            sf::FloatRect xBounds = xLabel.getLocalBounds();
            xLabel.setOrigin(sf::Vector2f(xBounds.size.x / 2.f, 0.f));
            xLabel.setPosition(sf::Vector2f(W/2.f, static_cast<float>(H) - BOTTOM_MARGIN/2.f));
            window.draw(xLabel);

            sf::Text yLabel(font, "Population", 14u);
            yLabel.setFillColor(sf::Color::Black);
            sf::FloatRect yBounds = yLabel.getLocalBounds();
            yLabel.setOrigin(sf::Vector2f(yBounds.size.x / 2.f, yBounds.size.y / 2.f));
            yLabel.setPosition(sf::Vector2f(15.f, H/2.f));
            yLabel.setRotation(sf::degrees(-90.f));
            window.draw(yLabel);

            float legendX = W - RIGHT_MARGIN - 180.f;
            float legendY = TOP_MARGIN + 20.f;
            const float legendHeight = 18.f;
            struct LegendItem { const char* name; sf::Color color; } items[] = {
                {"S (susceptible)", sf::Color::Blue},
                {"E (exposed)", sf::Color(255, 191, 0)},
                {"I (infected)", sf::Color(192, 76, 42)},
                {"R (recovered)", sf::Color(0, 128, 128)}
            };

            for (int i = 0; i < 4; ++i) {
                sf::RectangleShape swatch(sf::Vector2f(24.f, 8.f));
                swatch.setFillColor(items[i].color);
                swatch.setPosition(sf::Vector2f(legendX, legendY + i * legendHeight));
                window.draw(swatch);

                sf::Text text(font, items[i].name, 14u);
                text.setFillColor(sf::Color::Black);
                text.setPosition(sf::Vector2f(legendX + 32.f, legendY - 4.f + i * legendHeight));
                window.draw(text);
            }
        }

        window.display();
    }
    
    return 0;
}
