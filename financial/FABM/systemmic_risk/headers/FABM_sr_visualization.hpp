#pragma once
#include "FABM_sr_functions.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

static const float WINDOW_WIDTH = 1200.f;
static const float WINDOW_HEIGHT = 800.f;
static const float WINDOW_MARGIN = 100.f;
static const float NODE_RADIUS = 12.f;

inline sf::Color getStateColor(const State s) {
    switch (s) {
        case State::Solvent:
            return sf::Color(55, 138, 221);
        case State::Stressed:
            return sf::Color(186, 117, 23);
        case State::Defaulted:
            return sf::Color(192, 76, 42);
        default:
            return sf::Color::White;
    }
}

inline std::string getStateLabel(const State s) {
    switch (s) {
        case State::Solvent:
            return "Solvent";
        case State::Stressed:
            return "Stressed";
        case State::Defaulted:
            return "Defaulted";
        default:
            return "Unknown";
    }
}

inline void runVisualization(std::vector<Banks>& banks,
                              std::vector<std::vector<int>>& contacts,
                              std::vector<std::vector<double>>& exposure,
                              std::mt19937& rng,
                              const BankParams& params,
                              int totalPeriods) {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u{static_cast<unsigned int>(WINDOW_WIDTH), static_cast<unsigned int>(WINDOW_HEIGHT)}), "Systemic Risk Network");
    window.setFramerateLimit(60);

    const int N = static_cast<int>(banks.size());
    std::vector<sf::Vector2f> positions(N);
    std::vector<sf::CircleShape> nodes(N);
    std::vector<sf::Text> labels;
    labels.reserve(N);

    const float centerX = WINDOW_WIDTH * 0.5f;
    const float centerY = WINDOW_HEIGHT * 0.5f;
    const float radius = std::min(WINDOW_WIDTH, WINDOW_HEIGHT) * 0.30f;
    const double pi = 3.14159265358979323846;

    double maxExposure = 0.0;
    for (const auto& row : exposure) {
        for (const double value : row) {
            maxExposure = std::max(maxExposure, value);
        }
    }

    for (int i = 0; i < N; ++i) {
        const float angle = static_cast<float>(2.0 * pi * i) / static_cast<float>(std::max(1, N));
        positions[i] = sf::Vector2f(centerX + std::cos(angle) * radius,
                                    centerY + std::sin(angle) * radius);
    }

    for (int i = 0; i < N; ++i) {
        nodes[i].setRadius(NODE_RADIUS);
        nodes[i].setOrigin(sf::Vector2f(NODE_RADIUS, NODE_RADIUS));
        nodes[i].setPosition(positions[i]);
        nodes[i].setFillColor(getStateColor(banks[i].current_state));
        nodes[i].setOutlineThickness(0.f);
        nodes[i].setOutlineColor(sf::Color::White);
    }

    sf::Font font;
    if (!font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cerr << "Warning: Could not load font from C:\\Windows\\Fonts\\arial.ttf" << std::endl;
    }

    sf::Text titleText(font, "Systemic Risk Network", 16);
    sf::Text statusText(font, "", 14);
    sf::Text selectedText(font, "", 14);
    sf::Text pausedText(font, "", 16);
    sf::Text periodText(font, "", 14);

    titleText.setPosition(sf::Vector2f(10.f, 10.f));
    titleText.setFillColor(sf::Color::Black);
    statusText.setPosition(sf::Vector2f(10.f, 32.f));
    statusText.setFillColor(sf::Color::Black);
    selectedText.setPosition(sf::Vector2f(10.f, 54.f));
    selectedText.setFillColor(sf::Color::Black);
    periodText.setPosition(sf::Vector2f(10.f, 76.f));
    periodText.setFillColor(sf::Color::Black);
    pausedText.setPosition(sf::Vector2f(10.f, 104.f));
    pausedText.setFillColor(sf::Color::Red);

    for (int i = 0; i < N; ++i) {
        labels.emplace_back(font, std::to_string(banks[i].id), 10);
        labels.back().setFillColor(sf::Color::Black);
        labels.back().setPosition(sf::Vector2f(positions[i].x - 4.f, positions[i].y - 6.f));
    }

    sf::Clock clock;
    int currentPeriod = 0;
    float secondsPerPeriod = 0.65f;
    bool paused = false;
    int selectedBank = -1;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Space) {
                    paused = !paused;
                }
            }

            if (auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    selectedBank = -1;
                    for (int i = 0; i < N; ++i) {
                        const float dist = std::hypot(mousePos.x - positions[i].x, mousePos.y - positions[i].y);
                        if (dist <= NODE_RADIUS + 3.f) {
                            selectedBank = i;
                            break;
                        }
                    }
                }
            }
        }

        if (!paused && clock.getElapsedTime().asSeconds() > secondsPerPeriod && currentPeriod < totalPeriods) {
            update_state(banks, exposure, N);
            ++currentPeriod;
            clock.restart();
        }

        int s = count_state(banks, State::Solvent);
        int t = count_state(banks, State::Stressed);
        int d = count_state(banks, State::Defaulted);

        if (selectedBank >= 0 && selectedBank < N) {
            const Banks& bank = banks[selectedBank];
            std::ostringstream info;
            info << std::fixed << std::setprecision(2);
            info << "Selected bank: ID " << bank.id
                 << " | Equity: " << bank.equity
                 << " | Total assets: " << bank.total_assets
                 << " | State: " << getStateLabel(bank.current_state);
            selectedText.setString(info.str());
        } else {
            selectedText.setString("Click a node to inspect bank details.");
        }

        statusText.setString("Solvent: " + std::to_string(s)
                            + "  Stressed: " + std::to_string(t)
                            + "  Defaulted: " + std::to_string(d));

        periodText.setString("Period: " + std::to_string(currentPeriod));

        if (paused) {
            pausedText.setString("[PAUSED]");
        } else {
            pausedText.setString("");
        }

        window.clear(sf::Color(245, 245, 240));

        for (int i = 0; i < N; ++i) {
            for (size_t k = 0; k < contacts[i].size(); ++k) {
                const int j = contacts[i][k];
                if (j <= i) {
                    continue;
                }

                const bool iStressedOrDefaulted = (banks[i].current_state == State::Stressed || banks[i].current_state == State::Defaulted);
                const bool jStressedOrDefaulted = (banks[j].current_state == State::Stressed || banks[j].current_state == State::Defaulted);

                if (!iStressedOrDefaulted && !jStressedOrDefaulted) {
                    continue;
                }

                const double exposureValue = (k < exposure[i].size()) ? exposure[i][k] : 0.0;
                const double normalized = (maxExposure > 0.0) ? std::clamp(exposureValue / maxExposure, 0.0, 1.0) : 0.0;
                const int red = static_cast<int>(140.0 + 70.0 * normalized);
                const int green = static_cast<int>(60.0 + 25.0 * (1.0 - normalized));
                const int blue = static_cast<int>(55.0 + 25.0 * (1.0 - normalized));

                sf::Vertex line[2];
                line[0].position = positions[i];
                line[1].position = positions[j];
                line[0].color = sf::Color(static_cast<std::uint8_t>(red), static_cast<std::uint8_t>(green), static_cast<std::uint8_t>(blue), 140);
                line[1].color = sf::Color(static_cast<std::uint8_t>(red), static_cast<std::uint8_t>(green), static_cast<std::uint8_t>(blue), 140);
                window.draw(line, 2u, sf::PrimitiveType::Lines);
            }
        }

        for (int i = 0; i < N; ++i) {
            nodes[i].setPosition(positions[i]);
            nodes[i].setFillColor(getStateColor(banks[i].current_state));
            nodes[i].setOutlineThickness(selectedBank == i ? 3.f : 0.f);
            window.draw(nodes[i]);
            labels[i].setPosition(sf::Vector2f(positions[i].x - 6.f, positions[i].y - 7.f));
            window.draw(labels[i]);
        }

        window.draw(titleText);
        window.draw(statusText);
        window.draw(selectedText);
        window.draw(periodText);
        window.draw(pausedText);
        window.display();
    }
}
