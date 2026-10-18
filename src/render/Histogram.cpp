#include "render/Histogram.hpp"

void drawHistogram(sf::RenderWindow& window, const std::vector<int>& data, int max) {
    if (data.empty() || max <= 0) return;

    int numBucketsTotal = (max + 10) / 10;
    std::vector<int> counts(numBucketsTotal, 0);

    for (int x : data) {
        if (x < max && x > 0) {
            int bucket = x / 10;
            counts[bucket]++;
        }
    }

    int maxFreq = 0;
    for (int freq : counts) {
        if (freq > maxFreq) maxFreq = freq;
    }
    if (maxFreq == 0) return;

    const float padding = 60.f;
    auto windowSize = window.getSize();
    if (windowSize.x <= padding * 2 || windowSize.y <= padding * 2) return;

    const sf::Vector2f chartSize(windowSize.x - padding * 2, windowSize.y - padding * 2);
    const sf::Vector2f offset(padding, windowSize.y - padding);

    sf::VertexArray va(sf::PrimitiveType::Triangles);
    float barWidth = chartSize.x / numBucketsTotal;

    sf::Color barColor = sf::Color::Cyan;

    for (int val = 0; val < numBucketsTotal; val++) {
        int freq = counts[val];
        if (freq == 0) continue;
        float h = (static_cast<float>(freq) / maxFreq) * chartSize.y;
        float xPos = val * barWidth;

        va.append({{offset.x + xPos, offset.y}, barColor});
        va.append({{offset.x + xPos, offset.y - h}, barColor});
        va.append({{offset.x + xPos + barWidth, offset.y - h}, barColor});

        va.append({{offset.x + xPos, offset.y}, barColor});
        va.append({{offset.x + xPos + barWidth, offset.y - h}, barColor});
        va.append({{offset.x + xPos + barWidth, offset.y}, barColor});
    }

    window.draw(va);

    const sf::Font font("assets/font/times.ttf");
    sf::Text label(font, "");
    label.setCharacterSize(14);
    label.setFillColor(sf::Color::White);

    for (int i = 0; i <= 5; ++i) {
        int val = (maxFreq * i) / 5;
        float yPos = offset.y - ((static_cast<float>(val) / maxFreq) * chartSize.y);

        label.setString(std::to_string(val));
        label.setPosition({offset.x - 40.f, yPos - 10.f});
        window.draw(label);
    }

    std::vector<int> xMarkers;
    int numMarkers = 5;
    for (int i = 0; i < numMarkers; i++) {
        int val = i * (numBucketsTotal - 1) / (numMarkers - 1);
        xMarkers.push_back(val);
    }
    for (int val : xMarkers) {
        float xPos = offset.x + val * barWidth;
        int start = val * 10;
        int end = start + 9;
        std::string labelStr = std::to_string(start) + "-" + std::to_string(end);
        label.setString(labelStr);
        label.setPosition({xPos, offset.y + 10.f});
        window.draw(label);
    }
}
