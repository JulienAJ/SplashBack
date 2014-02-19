#include <iostream>
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
	for(int i = 0; i < lines; i++)
		for(int y = 0; y < lines; y++)
			board[i][y] = 0;
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

	//while(!empty && shots > 0) A remplacer apres avoir refait fonction generer
	while(shots > 0)
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
		for(int y = 0; y < lines; y++)
			if(board[i][y] != 0)
				return false;

	return true;
}

void Splash::action(int line, int column)
{
	if(board[line][column] == 3)
	{
		explode(line, column);
	}

	else
		board[line][column]++;

	shots--;
}

void Splash::explode(int line, int column)
{
	std::cout << std::endl << "left" << std::endl;
	left(line, column);
	std::cout << std::endl << "right" << std::endl;
	right(line, column);
	std::cout << std::endl << "top" << std::endl;
	top(line, column);
	std::cout << std::endl << "bottom" << std::endl;
	bottom(line, column);
	std::cout << std::endl << "end" << std::endl;

	board[line][column] = 0;
	shots++;
}

void Splash::left(int line, int column)
{
	if(column == 0)
		return;

	bool done = false;
	int i = column-1;

	while(!done && i >= 0)
	{
		if(board[line][i] == 3)
		{
			explode(line, i);
			done = true;
		}

		else if (board[line][i] != 0)
		{
			board[line][i]++;
			done = true;
		}
		i--;
	}
}

void Splash::right(int line, int column)
{
	if(column == columns-1)
		return;

	bool done = false;
	int i = column+1;

	while(!done && i < columns)
	{
		if(board[line][i] == 3)
		{
			explode(line, i);
			done = true;
		}

		else if (board[line][i] != 0)
		{
			board[line][i]++;
			done = true;
		}
		i++;
	}
}

void Splash::top(int line, int column)
{
	if(line == 0)
		return;

	bool done = false;
	int i = line-1;

	while(!done && i >= 0)
	{
		if(board[i][column] == 3)
		{
			explode(i, column);
			done = true;
		}

		else if (board[i][column] != 0)
		{
			board[i][column]++;
			done = true;
		}
		i--;
	}
}

void Splash::bottom(int line, int column)
{
	if(line == lines-1)
		return;

	bool done = false;
	int i = line+1;

	while(!done && i < lines)
	{
		if(board[i][column] == 3)
		{
			explode(i, column);
			done = true;
		}

		else if (board[i][column] != 0)
		{
			board[i][column]++;
			done = true;
		}
		i++;
	}
}