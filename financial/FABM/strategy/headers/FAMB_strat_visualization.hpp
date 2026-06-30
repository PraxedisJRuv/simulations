#pragma once

#include "FABM_strat_tests.hpp"
#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace fabm_strategy_vis {

inline double computeExcessKurtosis(const std::vector<double>& values) {
    if (values.size() < 4) {
        return 0.0;
    }

    const double mean = std::accumulate(values.begin(), values.end(), 0.0) / static_cast<double>(values.size());
    double m2 = 0.0;
    double m4 = 0.0;
    for (double v : values) {
        const double d = v - mean;
        const double d2 = d * d;
        m2 += d2;
        m4 += d2 * d2;
    }

    m2 /= static_cast<double>(values.size());
    m4 /= static_cast<double>(values.size());

    if (m2 <= 1e-12) {
        return 0.0;
    }

    const double kurtosis = m4 / (m2 * m2);
    return kurtosis - 3.0;
}

inline double gaussianPdf(double x, double mean, double sigma) {
    if (sigma <= 1e-12) {
        return (std::abs(x - mean) < 1e-12) ? 1e12 : 0.0;
    }
    const double coeff = 1.0 / (sigma * std::sqrt(2.0 * 3.14159265358979323846));
    const double exponent = -0.5 * std::pow((x - mean) / sigma, 2);
    return coeff * std::exp(exponent);
}

inline sf::Color strategyColor(int strategyIndex) {
    switch (strategyIndex) {
        case 0:
            return sf::Color(46, 134, 193);
        case 1:
            return sf::Color(245, 176, 65);
        case 2:
            return sf::Color(125, 206, 160);
        default:
            return sf::Color::Black;
    }
}

inline void drawAxes(sf::RenderTarget& target, const sf::FloatRect& panel, sf::Color bgColor = sf::Color(0, 0, 0, 0)) {
    const float left = panel.position.x;
    const float top = panel.position.y;
    const float width = panel.size.x;
    const float height = panel.size.y;

    sf::Vertex axisX[2];
    axisX[0].position = sf::Vector2f(left + 6.f, top + height - 28.f);
    axisX[0].color = sf::Color(66, 66, 66);
    axisX[1].position = sf::Vector2f(left + width - 8.f, top + height - 28.f);
    axisX[1].color = sf::Color(66, 66, 66);
    target.draw(axisX, 2, sf::PrimitiveType::Lines);

    sf::Vertex axisY[2];
    axisY[0].position = sf::Vector2f(left + 8.f, top + 6.f);
    axisY[0].color = sf::Color(66, 66, 66);
    axisY[1].position = sf::Vector2f(left + 8.f, top + height - 28.f);
    axisY[1].color = sf::Color(66, 66, 66);
    target.draw(axisY, 2, sf::PrimitiveType::Lines);
}

inline void drawPanel(sf::RenderTarget& target, const sf::FloatRect& panel, const std::string& title, const sf::Font* font = nullptr, sf::Color fill = sf::Color(250, 250, 250), sf::Color border = sf::Color(220, 220, 220)) {
    sf::RectangleShape background(sf::Vector2f(panel.size.x, panel.size.y));
    background.setPosition(sf::Vector2f(panel.position.x, panel.position.y));
    background.setFillColor(fill);
    background.setOutlineColor(border);
    background.setOutlineThickness(1.f);
    target.draw(background);

    if (font != nullptr) {
        sf::Text text(*font, title, 14);
        text.setFillColor(sf::Color(60, 60, 60));
        text.setPosition(sf::Vector2f(panel.position.x + 8.f, panel.position.y + 6.f));
        target.draw(text);
    }
}

inline void plotPricePath(sf::RenderTarget& target, const sf::FloatRect& panel, const std::vector<double>& prices) {
    if (prices.empty()) {
        return;
    }

    const float left = panel.position.x + 12.f;
    const float right = panel.position.x + panel.size.x - 12.f;
    const float bottom = panel.position.y + panel.size.y - 28.f;
    const float top = panel.position.y + 24.f;
    const float plotW = right - left;
    const float plotH = bottom - top;

    double minP = *std::min_element(prices.begin(), prices.end());
    double maxP = *std::max_element(prices.begin(), prices.end());
    const double margin = std::max(1e-12, 0.08 * std::max(std::abs(maxP), std::abs(minP)));
    minP -= margin;
    maxP += margin;
    if (maxP <= minP) {
        maxP = minP + 1.0;
    }

    sf::VertexArray line(sf::PrimitiveType::LineStrip, prices.size());
    for (std::size_t i = 0; i < prices.size(); ++i) {
        const float x = left + static_cast<float>(i) / static_cast<float>(std::max<std::size_t>(1, prices.size() - 1)) * plotW;
        const double normalized = (prices[i] - minP) / (maxP - minP);
        const float y = bottom - static_cast<float>(normalized) * plotH;
        line[i].position = sf::Vector2f(x, y);
        line[i].color = sf::Color(46, 134, 193);
    }
    target.draw(line);

    for (int tick = 0; tick <= 4; ++tick) {
        const float x = left + static_cast<float>(tick) / 4.0f * plotW;
        sf::Vertex vLine[2];
        vLine[0].position = sf::Vector2f(x, bottom);
        vLine[0].color = sf::Color(200, 200, 200);
        vLine[1].position = sf::Vector2f(x, bottom + 5.f);
        vLine[1].color = sf::Color(200, 200, 200);
        target.draw(vLine, 2, sf::PrimitiveType::Lines);
    }
}

inline void plotStrategyComposition(sf::RenderTarget& target, const sf::Font* font, const sf::FloatRect& panel, const std::vector<std::array<int, 3>>& composition) {
    if (composition.empty()) {
        return;
    }

    const float left = panel.position.x + 12.f;
    const float right = panel.position.x + panel.size.x - 12.f;
    const float bottom = panel.position.y + panel.size.y - 28.f;
    const float top = panel.position.y + 24.f;
    const float plotW = right - left;
    const float plotH = bottom - top;

    int total = 0;
    for (const auto& row : composition) {
        for (int v : row) {
            total += v;
        }
    }
    const int maxBuyers = std::max(1, total);

    const std::size_t n = composition.size();
    for (std::size_t i = 0; i < n; ++i) {
        const float x = left + static_cast<float>(i) / static_cast<float>(std::max<std::size_t>(1, n - 1)) * plotW;
        const float width = std::max(4.f, plotW / static_cast<float>(std::max<std::size_t>(1, n)) * 0.7f);
        float y0 = bottom;

        for (int s = 0; s < 3; ++s) {
            const float height = static_cast<float>(composition[i][s]) / static_cast<float>(maxBuyers) * (plotH * 0.92f);
            sf::RectangleShape segment(sf::Vector2f(width, height));
            segment.setPosition(sf::Vector2f(x - width / 2.f, y0 - height));
            segment.setFillColor(strategyColor(s));
            target.draw(segment);
            y0 -= height;
        }
    }

    if (font != nullptr) {
        sf::Text legend(*font, "Fundamentalist  Chartist  Random", 11);
        legend.setFillColor(sf::Color(80, 80, 80));
        legend.setPosition(sf::Vector2f(panel.position.x + 14.f, panel.position.y + panel.size.y - 14.f));
        target.draw(legend);
    }
}

inline void plotWealthDistribution(sf::RenderTarget& target, const sf::FloatRect& panel, const std::vector<double>& wealths) {
    if (wealths.empty()) {
        return;
    }

    const float left = panel.position.x + 12.f;
    const float right = panel.position.x + panel.size.x - 12.f;
    const float bottom = panel.position.y + panel.size.y - 28.f;
    const float top = panel.position.y + 24.f;
    const float plotW = right - left;
    const float plotH = bottom - top;

    double minW = *std::min_element(wealths.begin(), wealths.end());
    double maxW = *std::max_element(wealths.begin(), wealths.end());
    const double range = std::max(1e-12, maxW - minW);
    minW -= 0.02 * range;
    maxW += 0.02 * range;

    const int bins = 10;
    std::vector<int> counts(bins, 0);
    for (double w : wealths) {
        int bin = static_cast<int>(((w - minW) / std::max(1e-12, maxW - minW)) * bins);
        if (bin < 0) bin = 0;
        if (bin >= bins) bin = bins - 1;
        ++counts[bin];
    }

    const int maxCount = *std::max_element(counts.begin(), counts.end());
    for (int b = 0; b < bins; ++b) {
        const float barW = plotW / static_cast<float>(bins) - 2.f;
        const float x = left + static_cast<float>(b) * plotW / static_cast<float>(bins) + 1.f;
        const float h = maxCount > 0 ? (static_cast<float>(counts[b]) / static_cast<float>(maxCount)) * (plotH * 0.8f) : 0.f;
        const float y = bottom - h;

        sf::RectangleShape bar(sf::Vector2f(barW, h));
        bar.setPosition(sf::Vector2f(x, y));
        bar.setFillColor(sf::Color(46, 134, 193, 170));
        target.draw(bar);
    }
}

inline void plotReturnsVsNormal(sf::RenderTarget& target, const sf::FloatRect& panel, const std::vector<double>& returns) {
    if (returns.empty()) {
        return;
    }

    const float left = panel.position.x + 12.f;
    const float right = panel.position.x + panel.size.x - 12.f;
    const float bottom = panel.position.y + panel.size.y - 28.f;
    const float top = panel.position.y + 24.f;
    const float plotW = right - left;
    const float plotH = bottom - top;

    const double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / static_cast<double>(returns.size());
    double variance = 0.0;
    for (double r : returns) {
        const double d = r - mean;
        variance += d * d;
    }
    variance /= static_cast<double>(returns.size());
    const double sigma = std::sqrt(std::max(variance, 1e-12));

    const double minR = *std::min_element(returns.begin(), returns.end());
    const double maxR = *std::max_element(returns.begin(), returns.end());
    const double span = std::max(1e-12, maxR - minR);
    const double xMin = minR - 0.15 * span;
    const double xMax = maxR + 0.15 * span;

    const int bins = 12;
    std::vector<int> counts(bins, 0);
    for (double r : returns) {
        int bin = static_cast<int>(((r - xMin) / std::max(1e-12, xMax - xMin)) * bins);
        if (bin < 0) bin = 0;
        if (bin >= bins) bin = bins - 1;
        ++counts[bin];
    }

    const int maxCount = *std::max_element(counts.begin(), counts.end());
    for (int b = 0; b < bins; ++b) {
        const float barW = plotW / static_cast<float>(bins) - 2.f;
        const float x = left + static_cast<float>(b) * plotW / static_cast<float>(bins) + 1.f;
        const float h = maxCount > 0 ? (static_cast<float>(counts[b]) / static_cast<float>(maxCount)) * (plotH * 0.55f) : 0.f;
        sf::RectangleShape bar(sf::Vector2f(barW, std::max(1.f, h)));
        bar.setPosition(sf::Vector2f(x, bottom - h));
        bar.setFillColor(sf::Color(245, 176, 65, 130));
        target.draw(bar);
    }

    sf::VertexArray normalCurve(sf::PrimitiveType::LineStrip, 120);
    for (int i = 0; i < 120; ++i) {
        const double x = xMin + (xMax - xMin) * static_cast<double>(i) / 119.0;
        const double y = gaussianPdf(x, mean, sigma);
        const float px = left + static_cast<float>((x - xMin) / std::max(1e-12, xMax - xMin)) * plotW;
        const float py = bottom - static_cast<float>(std::min(1.0, y / std::max(1e-12, gaussianPdf(mean, mean, sigma))) * (plotH * 0.55f));
        normalCurve[i].position = sf::Vector2f(px, py);
        normalCurve[i].color = sf::Color(192, 76, 42);
    }
    target.draw(normalCurve);
}

inline void showStrategyDashboard(const StrategySimulationSummary& summary, const double fair_value) {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u{1280U, 900U}), "FABM Strategy Dashboard");
    window.setFramerateLimit(30);

    sf::Font font;
    const bool hasFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");

    const double currentPrice = summary.prices.empty() ? 0.0 : summary.prices.back();
    const double excessKurtosis = computeExcessKurtosis(summary.returns);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color(242, 242, 242));

        const sf::FloatRect panel1(sf::Vector2f(14.f, 12.f), sf::Vector2f(1252.f, 240.f));
        const sf::FloatRect panel2(sf::Vector2f(14.f, 264.f), sf::Vector2f(1252.f, 220.f));
        const sf::FloatRect panel3(sf::Vector2f(14.f, 496.f), sf::Vector2f(610.f, 300.f));
        const sf::FloatRect panel4(sf::Vector2f(636.f, 496.f), sf::Vector2f(630.f, 300.f));

        const sf::Font* fontPtr = hasFont ? &font : nullptr;

        drawPanel(window, panel1, "Price Path", fontPtr);
        drawPanel(window, panel2, "Strategy Composition over Time (Active Buyers by Type)", fontPtr);
        drawPanel(window, panel3, "Trader Wealth Distribution", fontPtr);
        drawPanel(window, panel4, "Daily Returns vs Normal Distribution", fontPtr);

        plotPricePath(window, panel1, summary.prices);
        plotStrategyComposition(window, fontPtr, panel2, summary.active_buyers_by_type);
        plotWealthDistribution(window, panel3, summary.trader_wealths);
        plotReturnsVsNormal(window, panel4, summary.returns);

        if (hasFont) {
            sf::Text summaryText(font, "", 18);
            summaryText.setFillColor(sf::Color(50, 50, 50));
            summaryText.setPosition(sf::Vector2f(24.f, 18.f));
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(4);
            oss << "Price: " << currentPrice << "   Excess Kurtosis: " << excessKurtosis;
            summaryText.setString(oss.str());
            window.draw(summaryText);

            sf::Text fairText(font, "", 14);
            fairText.setFillColor(sf::Color(80, 80, 80));
            fairText.setPosition(sf::Vector2f(22.f, 40.f));
            fairText.setString("Fair value: " + std::to_string(fair_value));
            window.draw(fairText);
        }

        window.display();
    }
}

} // namespace fabm_strategy_vis
