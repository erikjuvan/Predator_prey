#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime> 

const int SquareSize = 5;
const int Padding = 1;

const int SizeX = 100;
const int SizeY = 60;

const int WindowWidth = SizeX * SquareSize + (SizeX - 1) * Padding;
const int WindowWHeight = SizeY * SquareSize + (SizeY - 1) * Padding;

const int MaxHealth = 100;
const int DecreaseHealthBy = -10;
const int IncreaseHealthBy = 10;

int DelayTime = 30;

enum class CreatureType {
	NOTHING = 0,
	PREDATOR = 1,
	PREY = 2 
};

struct Tile {
	CreatureType creature;
	sf::RectangleShape rect;
	int health;	
	bool updated;

	void Init(int x, int y) {
		rect.setSize(sf::Vector2f(SquareSize, SquareSize));
		rect.setPosition(x * (SquareSize + Padding), y * (SquareSize + Padding));
		Clear();
	}

	void Clear() {
		health = 0;
		SetCreature(CreatureType::NOTHING);
		updated = false;
	}

	void SetCreature(const CreatureType t) {
		creature = t;
		switch (t)
		{
		case CreatureType::NOTHING:
			rect.setFillColor(sf::Color::Black);
			health = 0;
			break;
		case CreatureType::PREDATOR:
			rect.setFillColor(sf::Color::Red);
			break;
		case CreatureType::PREY:
			rect.setFillColor(sf::Color::Green);
			break;
		default:
			rect.setFillColor(sf::Color::White);
			break;
		}		
	}
};

struct Map {
	Tile tiles[SizeY][SizeX];

	void Randomize() {		
		for (int y = 0; y < SizeY; y++) {
			for (int x = 0; x < SizeX; x++) {
				int randVal = rand() % 1000;
				if (randVal > 200) {
					tiles[y][x].SetCreature(CreatureType::NOTHING);
				}					
				else if (randVal > 60) {
					tiles[y][x].SetCreature(CreatureType::PREY);
					tiles[y][x].health = 1;
					tiles[y][x].updated = false;
				}					
				else {
					tiles[y][x].SetCreature(CreatureType::PREDATOR);
					tiles[y][x].health = 100;
					tiles[y][x].updated = false;
				}
											
			}
		}		
	}

	void Init() {
		srand(time(NULL));
		for (int y = 0; y < SizeY; y++) {
			for (int x = 0; x < SizeX; x++) {
				tiles[y][x].Init(x, y);
			}
		}
	}

	void Clear(sf::RenderWindow& w) {
		for (int y = 0; y < SizeY; y++) {
			for (int x = 0; x < SizeX; x++) {
				tiles[y][x].Clear();				
			}
		}
		Draw(w);
		DelayTime = 50;
	}

	void Draw(sf::RenderWindow& w) {
		w.clear();

		for (int y = 0; y < SizeY; y++) {
			for (int x = 0; x < SizeX; x++) {
				if (tiles[y][x].creature != CreatureType::NOTHING)
					w.draw(tiles[y][x].rect);
			}
		}

		w.display();
	}

	void GenerateXY(int& newX, int& newY, const int x, const int y) {
		bool arr[3][3] = { false };

	again:	
		int randX = rand() % 3;
		int randY = rand() % 3;

		if (arr[randX][randY] == true)
			goto again;
		arr[randX][randY] = true;

		newX = x + (randX - 1);
		newY = y + (randY - 1);

		if (newX == x && newY == y) {
			newX = x;
			newY = y;
			return;
		}

		if (newX < 0) newX = 0;
		if (newX >= SizeX) newX = SizeX - 1;
		if (newY < 0) newY = 0;
		if (newY >= SizeY) newY = SizeY - 1;		

		if (tiles[y][x].creature == CreatureType::PREDATOR) {
			if (tiles[newY][newX].creature == CreatureType::NOTHING || tiles[newY][newX].creature == CreatureType::PREY) {
				return;
			}
			else {
				goto again;
			}
		}

		else if (tiles[y][x].creature == CreatureType::PREY) {
			if (tiles[newY][newX].creature == CreatureType::NOTHING) {
				return;
			}
			else {
				goto again;
			}
		}
	}

	void Simulate() {		
		for (int y = 0; y < SizeY; y++) {
			for (int x = 0; x < SizeX; x++) {
				if (tiles[y][x].creature == CreatureType::NOTHING || tiles[y][x].updated == true) continue;

				int newX;
				int newY;

				GenerateXY(newX, newY, x, y);
				
				if (tiles[y][x].creature == CreatureType::PREY) {
					tiles[y][x].health += IncreaseHealthBy;
					if (tiles[y][x].health >= (MaxHealth + (rand() % 11))) {
						tiles[y][x].health = 1;
						tiles[newY][newX].SetCreature(CreatureType::PREY);
						tiles[newY][newX].health = 1;
					}
					else {
						if (newX != x || newY != y) {	// Moved
							tiles[newY][newX].SetCreature(CreatureType::PREY);
							tiles[newY][newX].health = tiles[y][x].health;
							tiles[y][x].SetCreature(CreatureType::NOTHING);
						}
					}
					tiles[newY][newX].updated = true;
				}

				else if (tiles[y][x].creature == CreatureType::PREDATOR) {
					if (tiles[newY][newX].creature == CreatureType::PREY) {
						tiles[newY][newX].SetCreature(CreatureType::PREDATOR);
						tiles[y][x].health = MaxHealth;
						tiles[newY][newX].health = MaxHealth / 2;
						tiles[newY][newX].updated = true;
					}
					else {
						tiles[y][x].health += DecreaseHealthBy;
						if (tiles[y][x].health <= 0) {
							tiles[y][x].SetCreature(CreatureType::NOTHING);
						}
						else {
							tiles[newY][newX].health = tiles[y][x].health;
							if (newX != x || newY != y) {	// Moved
								tiles[newY][newX].SetCreature(CreatureType::PREDATOR);
								tiles[y][x].SetCreature(CreatureType::NOTHING);
								tiles[newY][newX].updated = true;
							}
						}						
					}															
				}
			}
		}

		// Clear update flag
		for (int y = 0; y < SizeY; y++) {
			for (int x = 0; x < SizeX; x++) {
				tiles[y][x].updated = false;
			}
		}
	}
};

Map map;

int main()
{
	sf::RenderWindow windowMain(sf::VideoMode(WindowWidth, WindowWHeight), "Predator and prey", sf::Style::Close);

	map.Init();
	windowMain.clear();
	windowMain.display();

	bool start = false;
	bool singleStep = false;

	while (windowMain.isOpen()) {
		sf::Event event;
		while (windowMain.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				windowMain.close();
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape)
					windowMain.close();
				if (event.key.code == sf::Keyboard::Space) {
					if (start == false)
						start = true;
					else
						start = false;
				}
				else if (event.key.code == sf::Keyboard::S) {
					singleStep = true;
				}
				else if (event.key.code == sf::Keyboard::C) {
					start = false;
					map.Clear(windowMain);
				}
				else if (event.key.code == sf::Keyboard::R) {
					map.Randomize();
					map.Draw(windowMain);
				}
				else if (event.key.code == sf::Keyboard::Up) {
					if (DelayTime >= 10) {
						DelayTime -= 5;
					}
				}
				else if (event.key.code == sf::Keyboard::Down) {
					DelayTime += 5;
				}
			}
		}

		if (start || singleStep) {
			// Delay
			sf::Clock clk;
			sf::Time t1;
			clk.restart();
			do {
				t1 = clk.getElapsedTime();
			} while (t1.asMilliseconds() < DelayTime);

			map.Simulate();
			map.Draw(windowMain);
			singleStep = false;
		}
	}

	return 0;
}