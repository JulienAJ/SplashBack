#ifndef SPLASH_HPP
#define SPLASH_HPP

#include <list>
#include <stdint.h>

struct Bullets
{
	std::pair<int, int> source;
	std::pair<int, int> finalPosition[4];
	int lastComboLevel;
};

static const int deltas[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

class Splash
{
	private:
		int** board;
		int shots;
		const int initialShots;
		int level;
		int currentComboLevel;
		std::list<Bullets> bulletsList;


		void handle_action_cli(int, int); //L, C
		void move_bullets();

		Bullets explode(int, int);
		void handle_action(int, int);

		uint32_t binDump();
		void restoreBoard(uint32_t);
		bool solveBT(std::list<std::pair<int, int> >&, int);

	public:
		Splash();
		Splash(int); // S
		~Splash();

		void play();
		void display();
		bool empty();
		void generate();
		int getCell(int, int);
		int getLevel();
		void nextLevel();
		void restart();

		bool solve(std::list<std::pair<int, int> >&);

		static bool inBoard(int, int);
		static bool onEdge(int, int, int);
		std::pair<int, int> getFinalPosition(int, int, int);

		void action_cli(int, int, bool = true); // L, C
		Bullets action(int, int, bool = true, int = 0); // L, C

		int getShots();
};

#endif
