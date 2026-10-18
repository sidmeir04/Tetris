#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include <SFML/Graphics.hpp>
#include <vector>

void drawHistogram(sf::RenderWindow& window, const std::vector<int>& data, int max);

#endif
