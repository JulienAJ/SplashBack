#ifndef SPLASH_HPP
#define SPLASH_HPP

#include <list>
#include <stdint.h>

class Splash
{
	private:
		int** board;
		int shots;
		int level;

		void generate();
		void action(int, int); // L, C
		void explode(int, int); // L, C
		void handle_action(int, int); //L, C

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

		bool solve(std::list<std::pair<int, int> >&);
};

#endif
