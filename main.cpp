#include "main.h"

const int windowWidth = 800;
const int windowHeight = 600;

const double baseWidth = 155;
const double rodHeight = 200;

const double horizontalPadding = (windowWidth - baseWidth * 3) / 4;
const double verticalPadding = (windowHeight - rodHeight) / 2;

vector < vector < sf::RectangleShape>> bricks(3);
vector<vector<int>> moves;

int main() {
	sf::RenderWindow window;
	createWindow(window);

	sf::Image icon;
	if (!icon.loadFromMemory(&icon_png, icon_png_len)) return EXIT_FAILURE;
	window.setIcon(32, 32, icon.getPixelsPtr());

	startGame(window);

	return EXIT_SUCCESS;
}

void createWindow(sf::RenderWindow& window) {
	window.create(sf::VideoMode(windowWidth, windowHeight, 32), "Tower of Hanoi",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);
}

sf::Text createPauseMessage(sf::Font& font, int height) {
	sf::Text pauseMessage;

	pauseMessage.setFont(font);
	pauseMessage.setCharacterSize(20);
	pauseMessage.setPosition(170.f, 150.f);
	pauseMessage.setFillColor(sf::Color::White);
	changeMessageHeight(pauseMessage, height);

	return pauseMessage;
}

void changeMessageHeight(sf::Text& pauseMessage, int height) {
	pauseMessage.setString("Welcome to Tower of Hanoi!\nYou can change tower height with arrow keys.\nCurrent height: " + to_string(height) + "\nThen press Enter to start!");
}

void startGame(sf::RenderWindow& window) {
	int height = 5;

	sf::Font font;
	if (!font.loadFromMemory(sansation_ttf, sansation_ttf_len)) window.close();
	sf::Text pauseMessage = createPauseMessage(font, height);

	sf::Clock clock;
	bool isPlaying = false;
	bool isMoving = false;

	sf::Vector2f from(0, 0);
	sf::Vector2f to(0, 0);

	while (window.isOpen()) {
		handleEvents(window, isPlaying, pauseMessage, clock, height);

		updateLogic(isPlaying, isMoving, from, to, clock);

		drawScreen(isPlaying, window, pauseMessage);
	}
}

void handleEvents(sf::RenderWindow& window, bool& isPlaying, sf::Text& pauseMessage, sf::Clock& clock, int& height) {
	sf::Event event;
	while (window.pollEvent(event)) {
		if ((event.type == sf::Event::Closed) || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			window.close();
			break;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			height = min(10, ++height);
			changeMessageHeight(pauseMessage, height);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			height = max(3, --height);
			changeMessageHeight(pauseMessage, height);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			if (!isPlaying) {
				// (re)start the game
				sf::RectangleShape brick;
				brick.setFillColor(sf::Color(60, 60, 60));
				brick.setOutlineColor(sf::Color(180, 180, 180));
				brick.setOutlineThickness(1);

				for (int i = height; i > 0; i--) {
					double x = 45 + i * 10;
					double y = 18;

					sf::Vector2f basePosition = rodBaseCenter(0);

					brick.setSize(sf::Vector2f(x, y));
					brick.setOrigin((x - 5) / 2, y + 1);
					brick.setPosition(basePosition.x, basePosition.y - (height - i) * (y + 1));

					bricks[0].push_back(brick);
				}

				bricks[1].clear();
				bricks[2].clear();

				moveTower(height, 0, 2, 1);

				isPlaying = true;
				clock.restart();
			}
		}
	}
}

sf::Vector2f rodBaseCenter(int rod) {
	double rodBaseCenterX = (horizontalPadding + baseWidth) * (rod + 1) - (baseWidth - 5) / 2 - 5;
	double rodBaseCenterY = windowHeight - verticalPadding - 10;
	return sf::Vector2f(rodBaseCenterX, rodBaseCenterY);
}

void updateLogic(bool& isPlaying, bool& isMoving, sf::Vector2f& from, sf::Vector2f& to, sf::Clock& clock) {
	isPlaying = !moves.empty();
	if (isPlaying) {
		float deltaTime = clock.restart().asSeconds();

		sf::Vector2f movingVector = to - from;

		if (isMoving) {
			vector<int> move = moves.front();
			sf::RectangleShape& brick = bricks[move[0]].back();
			float remainingDistance = length(to - brick.getPosition());

			if (remainingDistance == 0) {
				isMoving = false;
			} else {
				if (remainingDistance <= 10) {
					brick.setPosition(to);
				} else {
					float speed = length(movingVector) * 1.5;

					movingVector = normalized(movingVector) * speed * deltaTime;

					brick.move(movingVector);
				}
			}
		} else {
			vector<int> move = moves.front();

			if (!moves.empty() && length(movingVector) != 0) {
				bricks[move[1]].push_back(bricks[move[0]].back());
				bricks[move[0]].pop_back();
				moves.erase(moves.begin());
			}

			if (!moves.empty()) {
				move = moves.front();

				from = bricks[move[0]].back().getPosition();
				to = rodBaseCenter(move[1]);
				to.y -= bricks[move[1]].size() * 19;
				isMoving = true;
			} else {
				from = sf::Vector2f(0, 0);
				to = from;
			}

		}
	}
}

float length(sf::Vector2f vector) {
	return sqrtf(powf(vector.x, 2) + powf(vector.y, 2));
}

sf::Vector2f normalized(sf::Vector2f vector) {
	float l = length(vector);
	if (l == 0) return vector;
	vector.x /= l;
	vector.y /= l;
	return vector;
}

void moveDisk(int from, int to) {
	moves.push_back({from, to});
}

void moveTower(int amount, int from, int to, int temp) {
	if (amount == 0) return;

	moveTower(amount - 1, from, temp, to);
	moveDisk(from, to);
	moveTower(amount - 1, temp, to, from);
}

void drawScreen(bool isPlaying, sf::RenderWindow& window, sf::Text& pauseMessage) {
	window.clear(sf::Color(33, 33, 33));

	if (isPlaying) {
		drawRods(window);
		drawBricks(window);
	} else {
		window.draw(pauseMessage);
	}

	window.display();
}

void drawRods(sf::RenderWindow& window) {
	sf::RectangleShape base(sf::Vector2f(baseWidth, 10));
	sf::RectangleShape rod(sf::Vector2f(5, rodHeight));

	base.setFillColor(sf::Color(222, 184, 135));
	rod.setFillColor(sf::Color(222, 184, 135));

	base.setOrigin(0, -rodHeight + 10);
	rod.setOrigin(-(baseWidth - 5) / 2, 0);

	for (int i = 0; i < 3; i++) {
		double x = horizontalPadding * (i + 1) + baseWidth * i;
		base.setPosition(x, verticalPadding);
		rod.setPosition(x, verticalPadding);

		window.draw(base);
		window.draw(rod);
	}
}

void drawBricks(sf::RenderWindow& window) {
	for (vector<sf::RectangleShape> tower : bricks) {
		for (sf::RectangleShape brick : tower) {
			window.draw(brick);
		}
	}
}

