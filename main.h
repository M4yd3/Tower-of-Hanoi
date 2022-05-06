#ifndef MAIN_H_
#define MAIN_H_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace std;

void createWindow(sf::RenderWindow& window);

sf::Text createPauseMessage(sf::Font& font, int height);

void changeMessageHeight(sf::Text& pauseMessage, int height);

void startGame(sf::RenderWindow& window);

void handleEvents(sf::RenderWindow& window, bool& isPlaying, sf::Text& pauseMessage, sf::Clock& clock, int& key);

sf::Vector2f rodBaseCenter(int rod);

void updateLogic(bool& isPlaying, bool& isMoving, sf::Vector2f& from, sf::Vector2f& to, sf::Clock& clock);

float length(sf::Vector2f vector);

sf::Vector2f normalized(sf::Vector2f vector);

void moveDisk(int from, int to);

void moveTower(int amount, int from, int to, int temp);

void drawScreen(bool isPlaying, sf::RenderWindow& window, sf::Text& pauseMessage);

void drawRods(sf::RenderWindow& window);

void drawBricks(sf::RenderWindow& window);

#endif // !MAIN_H_
