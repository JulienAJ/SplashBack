#ifndef SPLASH_HPP
#define SPLASH_HPP

#include <list>
#include <stdint.h>

struct Bullets
{
	std::pair<int, int> source;
	std::pair<int, int> finalPosition[4];
};

class Splash
{
	private:
		int** board;
		int shots;
		int level;

		void explode_cli(int, int); // L, C
		void handle_action_cli(int, int); //L, C

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

		bool solve(std::list<std::pair<int, int> >&);

		void action_cli(int, int); // L, C
		void action(int, int, Bullets&, bool = true); // L, C

		int getShots();
};

#endif
