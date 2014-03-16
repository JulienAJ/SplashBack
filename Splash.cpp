#include <iostream>
#include <cstdlib>
#include <ctime>
#include <list>
#include <stdint.h>
#include "Splash.hpp"

Splash::Splash()
{
	shots = 10;
	level = 1;

	board = new int*[4];
	for(int i = 0; i < 4; i++)
		board[i] = new int[4];

	generate();
}

Splash::Splash(int shots)
{
	this->shots = shots;
	level = 1;

	board = new int*[4];
	for(int i = 0; i < 4; i++)
		board[i] = new int[4];

	generate();
}

Splash::~Splash()
{
	for(int i = 0; i < 4; i++)
		delete [] board[i];

	delete [] board;
}

void Splash::generate()
{
	srand(time(NULL));

	for(int i = 0; i < 4; i++)
	{
		for(int y = 0; y < 4; y++)
		{
			board[i][y] = rand() % 4;
		}
	}
}

void Splash::display()
{
	std::cout << "Splash Back : Niveau " << level << '\n';
	
	for(int i = 0; i < 4; i++)
	{
		for(int y = 0; y < 4; y++)
			std::cout << board[i][y] << " | ";
		std::cout << '\n';
	}

	std::cout << shots << " coups restants\n";
}

void Splash::play()
{
	int l, c;
	bool quit = false;

	while(!quit)
	{
		while(!empty() && shots > 0)
		{
			display();
			l = -1;
			c = -1;
			std::cout << "Entrez la ligne de la case a activer : ";
			std::cin >> l;
			std::cout << "Entrez la colone de la case a activer : ";
			std::cin >> c;
			action_cli(l, c);
		}

		if(empty())
		{
			std::cout << "\nNiveau termine !\n\n";
			shots++;
			level++;
			generate();
		}

		else
		{
			std::cout << "Desole, vous avez perdu :'(\n";
			quit = true;
		}
	}
}

bool Splash::empty()
{
	for(int i = 0; i < 4; i++)
		for(int y = 0; y < 4; y++)
			if(board[i][y] != 0)
				return false;

	return true;
}

void Splash::action_cli(int line, int column)
{
	handle_action_cli(line, column);
	shots--;
}

void Splash::handle_action_cli(int line, int column)
{
	if(board[line][column] == 3)
		explode_cli(line, column);
	else
		board[line][column]++;
}

void Splash::explode_cli(int line, int column)
{
	const int deltas[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
	bool combo = false;

	board[line][column] = 0;
	for(int i = 0; i < 4; i++)
	{
		int l = line;
		int c = column;
		bool in_board;

		do
		{
			l += deltas[i][0];
			c += deltas[i][1];
		} while((in_board = (l >= 0 && l < 4 && c >= 0 && c < 4)) &&
				board[l][c] == 0);

		if(in_board)
		{
			if(board[l][c] == 3)
				combo = true;
			handle_action_cli(l, c);
		}
	}

	if(combo)
		shots++;
}

bool Splash::action(int line, int column)
{
	if(!next_explosions.empty())
		return false;

	handle_action(line, column);
	shots--;

	return true;
}

void Splash::handle_action(int line, int column)
{
	if(board[line][column] == 3)
		next_explosions.push_back(std::pair<int, int>(line, column));
	else
		board[line][column]++;
}

void Splash::explode(std::list<Bullet> &bullets)
{
	const int deltas[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
	bool combo = false;
	Bullet b;
	std::list<std::pair<int, int> >::iterator it;
	std::list<std::pair<int, int> > explosions(next_explosions);
	next_explosions.clear();

	for(it = explosions.begin(); it != explosions.end(); ++it)
	{
		int line = it->first;
		int column = it->second;
		b.source = std::pair<int, int>(line, column);

		board[line][column] = 0;
		for(int i = 0; i < 4; i++)
		{
			int l = line;
			int c = column;
			bool in_board;

			do
			{
				l += deltas[i][0];
				c += deltas[i][1];
			} while((in_board = (l >= 0 && l < 4 && c >= 0 && c < 4)) &&
					board[l][c] == 0);

			if(in_board)
			{
				if(board[l][c] == 3)
					combo = true;
				handle_action(l, c);

				std::pair<int, int> e(l, c);
				b.final_position[i] = e;

			}
			else
				b.final_position[i] =
					std::pair<int, int>(l-deltas[i][0], c-deltas[i][1]);
		}

		bullets.push_back(b);
	}

	if(combo)
		shots++;
}

bool Splash::fetch_bullets(std::list<Bullet> &bullets)
{
	bullets.clear();

	if(next_explosions.empty())
		return false;

	explode(bullets);
	return true;
}


bool Splash::solve(std::list<std::pair<int, int> > &solution)
{
	uint32_t dump = binDump();
	int shots_save = shots;

	solution.clear();
	bool ret = solveBT(solution, shots);
	restoreBoard(dump);
	shots = shots_save;

	return ret;
}

bool Splash::solveBT(std::list<std::pair<int, int> > &solution, int max_shots)
{
	if(empty())
		return true;

	if(shots == 0 || max_shots == 0)
		return false;

	uint32_t dump = binDump();
	int old_shots = shots;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			action_cli(i, j);
			solution.push_back(std::pair<int, int>(i, j));

			if(solveBT(solution, max_shots-1))
				return true;

			restoreBoard(dump);
			solution.pop_back();
			shots = old_shots;
		}
	}

	return false;
}

uint32_t Splash::binDump()
{
	uint32_t boardDump = 0;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			/*
			 * i*4+j: n°case
			 * 2: bits necessaires pour stocker la valeur d'une case (val max = 3)
			 */
			int shift = (i*4+j)*2;
			boardDump |= board[i][j]<<shift;
		}
	}

	return boardDump;
}

void Splash::restoreBoard(uint32_t dump)
{
	uint32_t mask = 3; // remplis 2 bits
	uint32_t tmp;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			int shift = (i*4+j)*2;

			tmp = dump&(mask<<shift);
			board[i][j] = tmp>>shift;
		}
	}
}
