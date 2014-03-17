#ifndef SPLASH_HPP
#define SPLASH_HPP

#include <list>
#include <stdint.h>

struct Bullet
{
	std::pair<int, int> source;
	std::pair<int, int> final_position[4];
};

class Splash
{
	private:
		int** board;
		int shots;
		int level;
		std::list<std::pair<int, int> > next_explosions;

		void generate();
		void action_cli(int, int); // L, C
		void explode_cli(int, int); // L, C
		void handle_action_cli(int, int); //L, C

		void explode(std::list<Bullet>&);
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
		int getCell(int, int);

		bool solve(std::list<std::pair<int, int> >&);

		bool action(int, int); // L, C
		bool fetch_bullets(std::list<Bullet> &);
};

#endif
