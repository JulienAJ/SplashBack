#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "Splash.hpp"

Splash::Splash()
{
	columns = 4;
	lines = 4;
	shots = 10;

	board = new int*[4];
	for(int i = 0; i < 4; i++)
		board[i] = new int[4];

	generate();
}

Splash::Splash(int lines, int columns, int shots)
{
	this->lines = lines;
	this->columns = columns;
	this->shots = shots;

	board = new int*[lines];
	for(int i = 0; i < lines; i++)
		board[i] = new int[columns];

	generate();
}

Splash::~Splash()
{
	for(int i = 0; i < lines; i++)
		delete [] board[i];

	delete [] board;
}

void Splash::generate()
{
	srand(time(NULL));

	for(int i = 0; i < lines; i++)
	{
		for(int y = 0; y < columns; y++)
		{
			board[i][y] = rand() % 4;
		}
	}
}

void Splash::display()
{
	std::cout << "Splash Back\n";
	
	for(int i = 0; i < lines; i++)
	{
		for(int y = 0; y < columns; y++)
			std::cout << board[i][y] << " | ";
		std::cout << '\n';
	}

	std::cout << shots << " coups restants\n";
}

void Splash::play()
{
	int l, c;

	while(!empty() && shots > 0) 
	{
		display();
		l = -1;
		c = -1;
		std::cout << "Entrez la ligne de la case a activer : ";
		std::cin >> l;
		std::cout << "Entrez la colone de la case a activer : ";
		std::cin >> c;
		action(l, c);
	}

	if(empty())
		std::cout << "Felicitations, vous avez gagne ! =D \n";

	else	
		std::cout << "Desole, vous avez perdu :'(\n";
}

bool Splash::empty()
{
	for(int i = 0; i < lines; i++)
		for(int y = 0; y < columns; y++)
			if(board[i][y] != 0)
				return false;

	return true;
}

void Splash::action(int line, int column)
{
	if(board[line][column] != 0)
	{
		handle_action(line, column);
		shots--;
	}
}

void Splash::handle_action(int line, int column)
{
	if(board[line][column] == 3)
		explode(line, column);
	else
		board[line][column]++;
}

void Splash::explode(int line, int column)
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
			handle_action(l, c);
		}
	}

	if(combo)
		shots++;
}
