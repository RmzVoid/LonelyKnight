#include <cstdio>
#include <iostream>

#include "ChessBoard.h"

int main()
{
	//
	// Level 1
	//
	std::cout << "*** LEVEL 1 ***" << std::endl;

	ChessBoard<> board;

	std::cout << board;

	std::vector<Cell> pathEmpty = {};
	std::vector<Cell> pathSingle = { { 1, 6 } };
	std::vector<Cell> pathValid = { { 1, 1 }, { 2, 3 } };
	std::vector<Cell> pathOut = { { 1, 1 }, { 2, 3 }, { 1, 1 }, { -1, 2 }, { 2, 3 } };
	std::vector<Cell> pathInvalid = { { 1, 1 }, { 4, 0 } };
	std::vector<Cell> pathLongValid = { { 0, 2 }, { 2, 3 }, { 3, 5 }, { 2, 7 } };

	std::cout << "pathEmpty valid: " << std::boolalpha << board.MoveKnight(pathEmpty) << std:: endl;
	std::cout << "pathSingle valid: " << std::boolalpha << board.MoveKnight(pathSingle) << std::endl;
	std::cout << "pathValid valid: " << std::boolalpha << board.MoveKnight(pathValid) << std::endl;
	std::cout << "pathOut valid: " << std::boolalpha << board.MoveKnight(pathOut) << std::endl;
	std::cout << "pathInvalid valid: " << std::boolalpha << board.MoveKnight(pathInvalid) << std::endl;
	std::cout << "pathLongValid valid: " << std::boolalpha << board.MoveKnight(pathLongValid) << std::endl;
	
	//
	// Level 2
	//
	std::cout << "*** LEVEL 2 ***" << std::endl;

	Cell origin = { 0, 0 };
	Cell target = { 7, 5 };
	std::vector<Cell> path;
	bool result = false;

	result = board.CalculateAnyPath(origin, target, path);

	if (result)
	{
		std::cout << "Any path: From " << origin << " to " << target << " found path with " << path.size() << " moves:" << std::endl;
		int delimiter = 0;
		for (const auto& p : path)
		{
			std::cout << p << " ";
			if (++delimiter % 5 == 0)
				std::cout << std::endl;
		}
	}

	std::cout << std::endl;

	//
	// Level 3
	//
	std::cout << "*** LEVEL 3 ***" << std::endl;

	result = board.CalculateShortestPath(origin, target, path);

	if (result)
	{
		std::cout << "Shortest path: From " << origin << " to " << target << " found path with " << path.size() << " moves:" << std::endl;
		int delimiter = 0;
		for (const auto& p : path)
		{
			std::cout << p << " ";
			if (++delimiter % 5 == 0)
				std::cout << std::endl;
		}
	}

	std::cout << std::endl;

	//
	// Level 4
	//
	std::cout << "*** LEVEL 4 ***" << std::endl;
	
	ChessBoard<32, 28> boardLevel4("board.txt");

	std::cout << boardLevel4;

	origin = { 2, 4 };
	target = { 31, 27 };

	result = boardLevel4.CalculateShortestPathAStar(origin, target, path);

	if (result)
	{
		std::cout << "Shortest path: From " << origin << " to " << target << " found path with " << path.size() << " moves:" << std::endl;
		int delimiter = 0;
		for (const auto& p : path)
		{
			std::cout << p << " ";
			if (++delimiter % 5 == 0)
				std::cout << std::endl;
		}
	}

	std::string cmd;
	std::cout << std::endl << "Type 'quit' to quit from program" << std::endl;
	std::cin >> cmd;
	return 0;
}