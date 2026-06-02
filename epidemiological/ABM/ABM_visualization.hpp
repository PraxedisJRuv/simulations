#pragma once
#include "ABM_functions.hpp"
#include<SFML/Graphics.hpp>

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

// Starts the SFML visualization and simulation loop.
// Parameters: delta timestep, N population size, T days to simulate,
// p contact probability, param epidemiological parameters
void runVisualization(double delta, int N, 
                        int T, double p, 
                        const Parameters& param,
                        std::mt19937 rng,
                        std::bernoulli_distribution advance_E,
                        std::bernoulli_distribution advance_I,
                        std::vector<std::vector<int>> contacts,
                        std::vector<Agent> agents){
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
}