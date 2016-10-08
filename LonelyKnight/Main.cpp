#include <cstdio>
#include <iostream>

#include "ChessBoard.h"

int main()
{
	ChessBoard<> board;

	/*
	std::vector<Cell> pathEmpty = {};
	std::vector<Cell> pathSingle = { { 1, 6 } };
	std::vector<Cell> pathValid = { { 1, 1 }, { 2, 3 } };
	std::vector<Cell> pathOut = { { 1, 1 }, { 2, 3 }, { 1, 1 }, { -1, 2 }, { 2, 3 } };
	std::vector<Cell> pathInvalid = { { 1, 1 }, { 4, 0 } };
	std::vector<Cell> pathLongValid = { { 0, 2 }, { 2, 3 }, { 3, 5 }, { 2, 7 } };

	bool isValid = false;

	std::cout << "pathEmpty valid: " << std::boolalpha << board.MoveKnight(pathEmpty) << std:: endl;
	std::cout << "pathSingle valid: " << std::boolalpha << board.MoveKnight(pathSingle, true) << std::endl;
	std::cout << "pathValid valid: " << std::boolalpha << board.MoveKnight(pathValid, true) << std::endl;
	std::cout << "pathOut valid: " << std::boolalpha << board.MoveKnight(pathOut, true) << std::endl;
	std::cout << "pathInvalid valid: " << std::boolalpha << board.MoveKnight(pathInvalid, true) << std::endl;
	std::cout << "pathLongValid valid: " << std::boolalpha << board.MoveKnight(pathLongValid, true) << std::endl;
	*/

	Cell origin = { 1, 0 };
	Cell target = { 7, 7 };
	std::vector<Cell> path;
	bool result = false;

	result = board.CalculateAnyPath(origin, target, path);

	if (result)
	{
		std::cout << "Any path: From [" << origin.x << ", " << origin.y << "] to [" << target.x << ", " << target.y << "] found path with " << path.size() << " moves:" << std::endl;
		int delimiter = 0;
		for (auto p : path)
		{
			std::cout << "[" << p.x << ", " << p.y << "] ";
			if (++delimiter % 5 == 0)
				std::cout << std::endl;
		}
	}

	std::cout << std::endl;

	result = board.CalculateShortestPath(origin, target, path);

	if (result)
	{
		std::cout << "Shortes path: From [" << origin.x << ", " << origin.y << "] to [" << target.x << ", " << target.y << "] found path with " << path.size() << " moves:" << std::endl;
		int delimiter = 0;
		for (auto p : path)
		{
			std::cout << "[" << p.x << ", " << p.y << "] ";
			if (++delimiter % 5 == 0)
				std::cout << std::endl;
		}
	}

	std::cout << std::endl;
	
	origin = { 3, 4 };
	target = { 6, 3 };

	result = board.CalculateAnyPath(origin, target, path);

	if (result)
	{
		std::cout << "Any path: From [" << origin.x << ", " << origin.y << "] to [" << target.x << ", " << target.y << "] found path with " << path.size() << " moves:" << std::endl;
		int delimiter = 0;
		for (auto p : path)
		{
			std::cout << "[" << p.x << ", " << p.y << "] ";
			if (++delimiter % 5 == 0)
				std::cout << std::endl;
		}
	}

	std::cout << std::endl;

	result = board.CalculateShortestPath(origin, target, path);

	if (result)
	{
		std::cout << "Shortest path: From [" << origin.x << ", " << origin.y << "] to [" << target.x << ", " << target.y << "] found path with " << path.size() << " moves:" << std::endl;
		int delimiter = 0;
		for (auto p : path)
		{
			std::cout << "[" << p.x << ", " << p.y << "] ";
			if (++delimiter % 5 == 0)
				std::cout << std::endl;
		}
	}

	return 0;
}