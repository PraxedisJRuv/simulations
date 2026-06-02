#pragma once
#include "SEIR_functions.hpp"
#include <SFML/Graphics.hpp>

const unsigned int W = 900;
const unsigned int H = 600;
const float LEFT_MARGIN = 110.f;
const float RIGHT_MARGIN = 140.f;
const float TOP_MARGIN = 100.f;
const float BOTTOM_MARGIN = 100.f;

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

void runVisualization(double T, double delta, State& s, Parameters& p){
    sf::RenderWindow window(sf::VideoMode({900, 600}), "SEIR Simulation");
    std::vector<State> results;
    results.push_back(s);
    const int numSteps = T/delta;

    for (int i = 0; i < numSteps; ++i) {
        results.push_back(rnd_change(delta, delta, s, p));
    }

    std::vector<double> times, S_curve, E_curve, I_curve, R_curve;
    times.reserve(results.size());
    S_curve.reserve(results.size());
    E_curve.reserve(results.size());
    I_curve.reserve(results.size());
    R_curve.reserve(results.size());

    for (size_t i = 0; i < results.size(); ++i) {
        times.push_back(i * delta);
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
    bool fontLoaded = font.openFromFile("C:\\Windows\\Fonts\\arial.ttf");

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
                label.setPosition(sf::Vector2f(x, static_cast<float>(H) - BOTTOM_MARGIN + 6.f));
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
                label.setPosition(sf::Vector2f(LEFT_MARGIN - 12.f, y));
                window.draw(label);
            }
        }

        if (fontLoaded) {
            sf::Text xLabel(font, "Days", 14u);
            xLabel.setFillColor(sf::Color::Black);
            sf::FloatRect xBounds = xLabel.getLocalBounds();
            xLabel.setOrigin(sf::Vector2f(xBounds.size.x / 2.f, 0.f));
            xLabel.setPosition(sf::Vector2f(W/2.f, static_cast<float>(H) - BOTTOM_MARGIN/2.f + 12.f));
            window.draw(xLabel);

            sf::Text yLabel(font, "Population", 14u);
            yLabel.setFillColor(sf::Color::Black);
            sf::FloatRect yBounds = yLabel.getLocalBounds();
            yLabel.setOrigin(sf::Vector2f(yBounds.size.x / 2.f, yBounds.size.y / 2.f));
            yLabel.setPosition(sf::Vector2f(24.f, H/2.f));
            yLabel.setRotation(sf::degrees(-90.f));
            window.draw(yLabel);

            float legendX = W - RIGHT_MARGIN + 10.f;
            float legendY = TOP_MARGIN + 20.f;
            const float legendWidth = RIGHT_MARGIN - 20.f;
            const float legendHeight = 18.f;
            sf::RectangleShape legendBg(sf::Vector2f(legendWidth, legendHeight * 4 + 16.f));
            legendBg.setFillColor(sf::Color(255, 255, 255, 230));
            legendBg.setOutlineColor(sf::Color(180, 180, 180));
            legendBg.setOutlineThickness(1.f);
            legendBg.setPosition(sf::Vector2f(legendX, legendY - 8.f));
            window.draw(legendBg);

            struct LegendItem { const char* name; sf::Color color; } items[] = {
                {"S (susceptible)", sf::Color::Blue},
                {"E (exposed)", sf::Color(255, 191, 0)},
                {"I (infected)", sf::Color(192, 76, 42)},
                {"R (recovered)", sf::Color(0, 128, 128)}
            };

            for (int i = 0; i < 4; ++i) {
                sf::RectangleShape swatch(sf::Vector2f(18.f, 8.f));
                swatch.setFillColor(items[i].color);
                swatch.setPosition(sf::Vector2f(legendX + 8.f, legendY + i * legendHeight));
                window.draw(swatch);

                sf::Text text(font, items[i].name, 12u);
                text.setFillColor(sf::Color::Black);
                text.setPosition(sf::Vector2f(legendX + 32.f, legendY - 4.f + i * legendHeight));
                window.draw(text);
            }
        }

        window.display();
    }
    
}