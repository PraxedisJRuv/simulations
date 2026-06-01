#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include<SFML/Graphics.hpp>

/*
simga is the incubation rate
gamma the recovery rate
*/
enum class State{S,E1,E2,E3,I1,I2,R};
const int n_E=3;
const int n_I=2;

std::ostream& operator<<(std::ostream& os, State state) {
    switch (state) {
        case State::S:  os <<"S"; break;
        case State::E1: os <<"E1"; break;
        case State::E2: os <<"E2"; break;
        case State::E3:  os <<"E3"; break;
        case State::I1: os <<"I1"; break;
        case State::I2: os <<"I2"; break;
        case State::R: os <<"R"; break;
        default:           os << "Unknown State"; break;
    }
    return os;
}
struct Parameters{
    float beta, gamma, sigma;
    Parameters( float beta, float gamma, float sigma){
        this->beta=beta;
        this->gamma=gamma;
        this->sigma=sigma;
    }
};

struct Agent{
    int id;
    State state;
    float time_in_state;

    void transition(Agent& a, std::mt19937& rng, Parameters param, double delta,
        std::bernoulli_distribution advance_E, std::bernoulli_distribution advance_I){
        
        switch(a.state){
            case State::S:{
                double prob=1.0-std::exp(-param.beta*delta);
                if(std::bernoulli_distribution(prob)(rng)){
                    a.state=State::E1;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                }break;
            case State::E1:
                if(advance_E(rng)){
                    a.state=State::E2;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            case State::E2:
                if(advance_E(rng)){
                    a.state=State::E3;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            case State::E3:
                if(advance_E(rng)){
                    a.state=State::I1;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            case State::I1:
                if(advance_I(rng)){
                    a.state=State::I2;
                    a.time_in_state=0;}
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            case State::I2:
                if(advance_I(rng)){
                    a.state=State::R;
                    a.time_in_state=0;} 
                else{a.time_in_state=a.time_in_state+delta;}
                break;
            
            default: 
                a.time_in_state=a.time_in_state+delta;
                break;
        }

    }
};


int count(const std::vector<Agent>& agents, State I){
    int count = 0;
    for (int i = 0; i < static_cast<int>(agents.size()); i++){
        if (agents[i].state == I){
            count += 1;
        }
    }
    return count;
}

std::vector<std::vector<int>> conctacts_list(int N, float p, std::mt19937 rng){
    std::vector<std::vector<int>> contacts(N);
    std::bernoulli_distribution edge(p);
    for (int i=0; i<N; i++){
        for(int j=i+1; j<N; j++){
            if(edge(rng)){
                contacts[i].push_back(j);
                contacts[j].push_back(i);
            }
        }
    }
    return contacts;
}

int degree(int i, std::vector<std::vector<int>>& c){
    return c[i].size();
}

inline bool isInfectious(State s) {
    return s == State::I1 || s == State::I2;
}

int infectious_neighbors(int i, const std::vector<std::vector<int>>& contacts, const std::vector<Agent>& agents){
    int count = 0;
    for (int neighbor : contacts[i]){
        if (isInfectious(agents[neighbor].state)){
            count += 1;
        }
    }
    return count;
}

 void transition_with_network(int i, std::vector<Agent> &a, Agent & agent, const std::vector<std::vector<int>> &c, 
                                std::mt19937& rng, const Parameters &param, const double &delta,
                                std::bernoulli_distribution advance_E, std::bernoulli_distribution advance_I){
        
        switch(agent.state){
            case State::S: {
                double prob=1.0-std::exp(-param.beta*delta);
                int exposure = infectious_neighbors(i, c, a);
                for (int j=0; j<exposure; j++){
                    if(std::bernoulli_distribution(prob)(rng)){
                        agent.state = State::E1;
                        agent.time_in_state = 0;
                        break;
                    }
                }
                if (agent.state != State::E1){
                    agent.time_in_state = agent.time_in_state + delta;
                }
            } break;
            case State::E1:
                if (advance_E(rng)){
                    agent.state = State::E2;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;
            case State::E2:
                if (advance_E(rng)){
                    agent.state = State::E3;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;
            case State::E3:
                if (advance_E(rng)){
                    agent.state = State::I1;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;
            case State::I1:
                if (advance_I(rng)){
                    agent.state = State::I2;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;
            case State::I2:
                if (advance_I(rng)){
                    agent.state = State::R;
                    agent.time_in_state = 0;
                } else {
                    agent.time_in_state = agent.time_in_state + delta;
                }
                break;
            default:
                agent.time_in_state = agent.time_in_state + delta;
                break;
        }
    }

std::vector<std::vector<int>> test_contact_list(){
    int N=200;
    float p=0.05;

    double mean=0;
    int max=0;
    int min=300;

    int aux=0;
    int sum=0;

    std::mt19937 rng(std::random_device{}());
    std::vector<std::vector<int>> contacts=conctacts_list(N, p, rng);
    for (int i=0; i<N; i++){
        aux=degree(i,contacts);
        sum=sum+aux;
        if(aux>max){max=aux;}
        if(aux<min){min=aux;}
    }
    mean=sum/N;

    std::cout<<min<<"\t"<<max<<"\t"<<mean<<"\n";
    
    return contacts;
}

std::vector<Agent> test_random_progress(){
    double delta=0.5;
    int N=200;
    Parameters param(0.2,0.01,0.4);
    float rate_E=n_E*param.sigma;
    float rate_I=n_I*param.gamma;
    
    std::mt19937 rng(std::random_device{}());

    std::bernoulli_distribution advance_E(1-std::exp(-rate_E*delta));
    std::bernoulli_distribution advance_I(1-std::exp(-rate_I*delta));

    std::vector<Agent> agents;
    for (int i=0; i<197; i++){
        agents.push_back({i,State::S,0});
    }
    for (int i=197; i<200; i++){
        agents.push_back({i,State::I1,0});
    }

    for (int j=0; j<30; j++){
    for (int i=0; i<agents.size(); i++){
        agents[i].transition(agents[i], rng, param, delta, advance_E, advance_I);
    }
   
        std::cout<<"S: "<<count(agents,State::S)
                 <<"\t E1:"<<count(agents,State::E1)
                 <<"\t E2:"<<count(agents,State::E2)
                 <<"\t E3:"<<count(agents,State::E3)
                 <<"\t I1:"<<count(agents,State::I1)
                 <<"\t I2:"<<count(agents,State::I2)
                 <<"\t R:"<<count(agents,State::R)
                 <<"\t";
        std::cout<<j<<"\n"; 

    }
    return agents;
}

void agents_progress_with_network(int &N, int& T, std::vector<Agent>& agents,
                                                    std::vector<std::vector<int>>& contacts,std::mt19937& rng,
                                                    Parameters &param, double &delta, 
                                                    std::bernoulli_distribution &advance_E,
                                                    std::bernoulli_distribution &advance_I){
    std::vector<Agent> agents_copy;
    for (int j=0; j<T; j++){
        agents_copy=agents;
        for (int i=0; i<N; i++){
            transition_with_network(i, agents_copy, agents[i],contacts,
                                    rng,param, delta, advance_E,advance_I);
        }
}
}

void test_progress_with_network(){
    double delta=0.5;
    int N=200;
    Parameters param(0.2,0.01,0.4);
    float rate_E=n_E*param.sigma;
    float rate_I=n_I*param.gamma;
    
    std::mt19937 rng(std::random_device{}());

    std::bernoulli_distribution advance_E(1-std::exp(-rate_E*delta));
    std::bernoulli_distribution advance_I(1-std::exp(-rate_I*delta));

    std::vector<Agent> agents;
    for (int i=0; i<197; i++){
        agents.push_back({i,State::S,0});
    }
    for (int i=197; i<200; i++){
        agents.push_back({i,State::I1,0});
    }

    
    float p_contact=0.05;
    std::vector<std::vector<int>> contacts=conctacts_list(N, p_contact, rng);

    std::vector<Agent> agents_copy;
    for (int j=0; j<30; j++){
        agents_copy=agents;
        for (int i=0; i<N; i++){
            transition_with_network(i, agents_copy, agents[i],contacts,
                                    rng,param, delta, advance_E,advance_I);
        }
            std::cout<<"S: "<<count(agents,State::S)
                 <<"\t E1:"<<count(agents,State::E1)
                 <<"\t E2:"<<count(agents,State::E2)
                 <<"\t E3:"<<count(agents,State::E3)
                 <<"\t I1:"<<count(agents,State::I1)
                 <<"\t I2:"<<count(agents,State::I2)
                 <<"\t R:"<<count(agents,State::R)
                 <<"\t";
            std::cout<<j<<"\n"; 
    }
}

static const float WINDOW_WIDTH = 1200.f;
static const float WINDOW_HEIGHT = 800.f;
static const float WINDOW_MARGIN = 80.f;
static const float NODE_RADIUS = 6.f;

sf::Color getStateColor(State s){
    switch(s){
        case State::S:  return sf::Color(55, 138, 221);
        case State::E1:
        case State::E2:
        case State::E3: return sf::Color(186, 117, 23);
        case State::I1:
        case State::I2: return sf::Color(192, 76, 42);
        case State::R:  return sf::Color(29, 158, 117);
        default:        return sf::Color::White;
    }
}

void simulateDay(std::vector<Agent>& agents,
                 const std::vector<std::vector<int>>& contacts,
                 std::mt19937& rng,
                 const Parameters& param,
                 double delta,
                 std::bernoulli_distribution& advance_E,
                 std::bernoulli_distribution& advance_I){
    std::vector<Agent> next = agents;
    int N = static_cast<int>(agents.size());
    for (int i = 0; i < N; i++){
        transition_with_network(i, agents, next[i], contacts, rng, param, delta, advance_E, advance_I);
    }
    agents.swap(next);
}

void applySpringLayout(std::vector<sf::Vector2f>& positions,
                       std::vector<sf::Vector2f>& velocities,
                       const std::vector<std::vector<int>>& contacts,
                       float dt){
    int N = static_cast<int>(positions.size());
    const float repulsionStrength = 18000.f;
    const float attractionStrength = 0.12f;
    const float desiredDistance = 120.f;

    for (int i = 0; i < N; i++){
        velocities[i] = sf::Vector2f(0.f, 0.f);
    }

    for (int i = 0; i < N; i++){
        for (int j = i + 1; j < N; j++){
            sf::Vector2f delta = positions[j] - positions[i];
            float dist2 = delta.x * delta.x + delta.y * delta.y + 0.01f;
            float dist = std::sqrt(dist2);
            sf::Vector2f direction = delta / dist;
            float repulsion = repulsionStrength / dist2;
            velocities[i] -= direction * repulsion;
            velocities[j] += direction * repulsion;
        }
    }

    for (int i = 0; i < N; i++){
        for (int j : contacts[i]){
            if (j <= i) continue;
            sf::Vector2f delta = positions[j] - positions[i];
            float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y) + 0.01f;
            sf::Vector2f direction = delta / dist;
            float attraction = (dist - desiredDistance) * attractionStrength;
            velocities[i] += direction * attraction;
            velocities[j] -= direction * attraction;
        }
    }

    for (int i = 0; i < N; i++){
        velocities[i] *= 0.80f;
        positions[i] += velocities[i] * dt;
        if (positions[i].x < WINDOW_MARGIN) positions[i].x = WINDOW_MARGIN;
        if (positions[i].x > WINDOW_WIDTH - WINDOW_MARGIN) positions[i].x = WINDOW_WIDTH - WINDOW_MARGIN;
        if (positions[i].y < WINDOW_MARGIN) positions[i].y = WINDOW_MARGIN;
        if (positions[i].y > WINDOW_HEIGHT - WINDOW_MARGIN) positions[i].y = WINDOW_HEIGHT - WINDOW_MARGIN;
    }
}

int main(){
    double delta = 0.5;
    int N = 200;
    int T = 200;
    float p = 0.04f;
    Parameters param(0.2f, 0.3f, 0.4f);
    float rate_E = n_E * param.sigma;
    float rate_I = n_I * param.gamma;

    std::mt19937 rng(std::random_device{}());
    std::bernoulli_distribution advance_E(1 - std::exp(-rate_E * delta));
    std::bernoulli_distribution advance_I(1 - std::exp(-rate_I * delta));

    std::vector<std::vector<int>> contacts = conctacts_list(N, p, rng);
    std::vector<Agent> agents;
    agents.reserve(N);
    for (int i = 0; i < N - 3; i++){
        agents.push_back({i, State::S, 0});
    }
    for (int i = N - 3; i < N; i++){
        agents.push_back({i, State::I1, 0});
    }

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u{static_cast<unsigned int>(WINDOW_WIDTH), static_cast<unsigned int>(WINDOW_HEIGHT)}), "ABM Infection Network");
    window.setFramerateLimit(60);

    std::uniform_real_distribution<float> randX(WINDOW_MARGIN, WINDOW_WIDTH - WINDOW_MARGIN);
    std::uniform_real_distribution<float> randY(WINDOW_MARGIN, WINDOW_HEIGHT - WINDOW_MARGIN);

    std::vector<sf::Vector2f> positions(N);
    std::vector<sf::Vector2f> velocities(N, sf::Vector2f(0.f, 0.f));
    std::vector<sf::CircleShape> shapes(N);
    for (int i = 0; i < N; i++){
        positions[i] = sf::Vector2f(randX(rng), randY(rng));
        shapes[i].setRadius(NODE_RADIUS);
        shapes[i].setOrigin(sf::Vector2f(NODE_RADIUS, NODE_RADIUS));
        shapes[i].setPosition(positions[i]);
        shapes[i].setFillColor(getStateColor(agents[i].state));
    }

    // Load font for UI text
    sf::Font font;
    if (!font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cerr << "Warning: Could not load font from C:\\Windows\\Fonts\\arial.ttf" << std::endl;
    }

    // UI text objects
    sf::Text dayText(font, "", 16);
    sf::Text statusText(font, "", 14);
    sf::Text pausedText(font, "", 16);

    dayText.setPosition(sf::Vector2f(10.f, 10.f));
    dayText.setFillColor(sf::Color::Black);

    statusText.setPosition(sf::Vector2f(10.f, 35.f));
    statusText.setFillColor(sf::Color::Black);

    pausedText.setPosition(sf::Vector2f(10.f, 110.f));
    pausedText.setFillColor(sf::Color::Red);

    sf::Clock clock;
    int currentDay = 0;
    float secondsPerDay = 0.35f;
    bool paused = false;

    while (window.isOpen()){
        while (auto event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
            if (auto* keyEvent = event->getIf<sf::Event::KeyPressed>()){
                if (keyEvent->code == sf::Keyboard::Key::Space){
                    paused = !paused;
                }
            }
        }

        if (!paused && clock.getElapsedTime().asSeconds() > secondsPerDay && currentDay < T){
            simulateDay(agents, contacts, rng, param, delta, advance_E, advance_I);
            currentDay += 1;
            clock.restart();
        }

        // Update UI text
        int s = count(agents, State::S);
        int e1 = count(agents, State::E1);
        int e2 = count(agents, State::E2);
        int e3 = count(agents, State::E3);
        int i1 = count(agents, State::I1);
        int i2 = count(agents, State::I2);
        int r = count(agents, State::R);

        dayText.setString("Day: " + std::to_string(currentDay));

        statusText.setString("S: " + std::to_string(s) + "  E: " + std::to_string(e1 + e2 + e3)
                           + "  I: " + std::to_string(i1 + i2) + "  R: " + std::to_string(r));

        if (paused){
            pausedText.setString("[PAUSED]");
        } else {
            pausedText.setString("");
        }

        window.setTitle("Day " + std::to_string(currentDay)
            + "  S=" + std::to_string(s)
            + " E=" + std::to_string(e1 + e2 + e3)
            + " I=" + std::to_string(i1 + i2)
            + " R=" + std::to_string(r));

        applySpringLayout(positions, velocities, contacts, 0.016f);

        window.clear(sf::Color(245, 245, 240));

        for (int i = 0; i < N; i++){
            for (int j : contacts[i]){
                if (j <= i) continue;
                if (!isInfectious(agents[i].state) && !isInfectious(agents[j].state)){
                    continue;
                }
                sf::Vertex line[2];
                line[0].position = positions[i];
                line[0].color = sf::Color(192, 76, 42, 120);
                line[1].position = positions[j];
                line[1].color = sf::Color(192, 76, 42, 120);
                window.draw(line, 2u, sf::PrimitiveType::Lines);
            }
        }

        for (int i = 0; i < N; i++){
            shapes[i].setPosition(positions[i]);
            shapes[i].setFillColor(getStateColor(agents[i].state));
            window.draw(shapes[i]);
        }

        // Draw UI panel text
        window.draw(dayText);
        window.draw(statusText);
        window.draw(pausedText);

        window.display();
    }

    return 0;
}