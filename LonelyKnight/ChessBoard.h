#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <iomanip>

enum class CellState : unsigned char { Free, Start, End, Knight, Water, Rock, Barrier, Teleport, Lava, PathPoint };

struct Cell
{
	int x;
	int y;

	bool operator==(const Cell& cell) const
	{
		return cell.x == x && cell.y == y;
	}

	bool operator!=(const Cell& cell) const
	{
		return cell.x != x || cell.y != y;
	}
};

struct Move
{
	int dx;
	int dy;
};

template <int dimX = 8, int dimY = 8> 
class ChessBoard
{
private:
	// Struct represents a graph's node
	struct KnightPossibleMoves
	{
		// for waves algorithm
		int waveMark;
		// move cost (for lava = 5, for water = 2)
		int cost;
		// path cost from origin (for A*)
		int range;
		// heuristic value (for A*)
		int h;
		// there we come from to this node (for A*)
		KnightPossibleMoves* parent;
		// node's coordinates on board
		Cell origin;
		// adjacent nodes for this node
		std::vector<Cell> possibleMove;

		KnightPossibleMoves(const Cell& position)
			: origin(position), waveMark(-1), cost(1), h(99999), parent(nullptr), range(0) {}
	};

	// possible moves of knigh't as offsets from position
	const static std::vector<Move> possibleKnightMoves;

	// Board's cells states array
	CellState cells[dimY][dimX];
	std::vector<Cell> teleports;

	// Board's graph for knight moves
	KnightPossibleMoves *boardGraph[dimY][dimX];

	// Checks if given coordinates belongs to chess board
	bool withinBoard(const Cell& c) const { return c.x >= 0 && c.y >= 0 && c.x < dimX && c.y < dimY; }

	// Check if given origin and target cells are valid knight's move
	bool isValidKnightMove(const Cell& o, const Cell& t) const
	{
		// is within board
		if (!withinBoard(t))
			return false;

		// is knight move
		if ((o.x - t.x) * (o.x - t.x) + (o.y - t.y) * (o.y - t.y) != 5)
			return false;

		// is target barrier or rock there knight not able to move
		if (cells[t.y][t.x] == CellState::Barrier || cells[t.y][t.x] == CellState::Rock)
			return false;

		// Check Barrier on the path, knight have 2 imaginary ways then move to target
		// So to completely block path we need barriers on both ways
		bool way1blocked = false;
		bool way2blocked = false;
		int xIncrement = o.x < t.x ? 1 : -1;
		int yIncrement = o.y < t.y ? 1 : -1;
		
		if (abs(t.x - o.x) > abs(t.y - o.y))
		{
			if (cells[o.y][o.x + xIncrement * 1] == CellState::Barrier)
				way1blocked = true;
			else
				if (cells[o.y][o.x + xIncrement * 2] == CellState::Barrier)
					way1blocked = true;

			if (cells[o.y + yIncrement][o.x] == CellState::Barrier)
				way2blocked = true;
			else
				if (cells[o.y + yIncrement][o.x + xIncrement] == CellState::Barrier)
					way2blocked = true;
		}
		else
		{
			if (cells[o.y + yIncrement * 1][o.x] == CellState::Barrier)
				way1blocked = true;
			else
				if (cells[o.y + yIncrement * 2][o.x] == CellState::Barrier)
					way1blocked = true;

			if (cells[o.y][o.x + xIncrement] == CellState::Barrier)
				way2blocked = true;
			else
				if (cells[o.y + yIncrement][o.x + xIncrement] == CellState::Barrier)
					way2blocked = true;
		}

		if (way1blocked && way2blocked)
			return false;

		return true;
	}

	// Get array of cells in which knight can move in one turn
	std::vector<Cell> getValidKnightMoves(const Cell& origin)
	{
		// Knight have maximum 8 moves
		std::vector<Cell> targets(8);

		// apply possible moves to origin to obtain adjacent cells
		std::transform(
			possibleKnightMoves.begin(), possibleKnightMoves.end(),
			targets.begin(),
			[origin](const Move& m) -> Cell { return{ origin.x + m.dx, origin.y + m.dy }; });

		// remove cells there knight can't pass
		targets.erase(
			std::remove_if(targets.begin(), targets.end(),
			[this, origin](const Cell& c) -> bool { return !isValidKnightMove(origin, c); }),
			targets.end());

		return targets;
	}

	// Set particular cells to given state
	// also checks for teleport cells
	void setCell(const Cell& c, CellState state)
	{ 
		if (withinBoard(c))
		{
			// only two teleports on board
			if (state == CellState::Teleport)
			{
				if (teleports.size() >= 2)
					cells[c.y][c.x] = CellState::Free;
				else
				{
					teleports.push_back(c);
					cells[c.y][c.x] = state;
				}
			}
			else
				cells[c.y][c.x] = state;
		}
	}

	// Resets board to initial state
	void reset() { memset(cells, static_cast<int>(CellState::Free), sizeof(CellState) * dimX * dimY); }

	// Build knight's moves graphs
	void buildMovesGraph()
	{
		// Init board.
		// Board have dimX * dimY graph nodes
		memset(boardGraph, 0, sizeof(KnightPossibleMoves*) * dimX * dimY);

		for (int x = 0; x < dimX; ++x)
			for (int y = 0; y < dimY; ++y)
			{
				// create node
				KnightPossibleMoves* m = new KnightPossibleMoves{ Cell{ x, y } };
				// Get valid knight's moves from cell
				m->possibleMove = getValidKnightMoves(m->origin);
				m->cost = cellCost(m->origin);
				// add node to graph
				boardGraph[y][x] = m;
			}
	}

	// Prepare nodes graph for algorithms
	void resetGraphNodes()
	{
		for (int x = 0; x < dimX; x++)
			for (int y = 0; y < dimY; y++)
				if (boardGraph[y][x] != nullptr)
				{
					boardGraph[y][x]->waveMark = -1;
					boardGraph[y][x]->range = 0;
					boardGraph[y][x]->h = 99999;
					boardGraph[y][x]->parent = nullptr;
				}
	}

	// loads chessboard from text file
	bool loadFromFile(const std::string& file)
	{
		std::ifstream fs(file);
		std::string line;

		for (int l = 0; l < dimY && !(fs.fail() || fs.bad()); l++)
		{
			std::getline(fs, line);
			int ci = 0;
			for (const auto& c : line)
			{
				switch (c)
				{
				case '.': setCell(Cell{ ci, l }, CellState::Free); break;
				case 'B': setCell(Cell{ ci, l }, CellState::Barrier); break;
				case 'L': setCell(Cell{ ci, l }, CellState::Lava); break;
				case 'R': setCell(Cell{ ci, l }, CellState::Rock); break;
				case 'W': setCell(Cell{ ci, l }, CellState::Water); break;
				case 'T': setCell(Cell{ ci, l }, CellState::Teleport);
				}

				if (c != ' ')
					ci++;
			}
		}

		return true;
	}

	// heuristic function for algorithm A* (distance between cells)
	int h(const Cell c0, const Cell& c1)
	{
		return static_cast<int>(sqrt((c0.x - c1.x)*(c0.x - c1.x) + (c0.y - c1.y)*(c0.y - c1.y)));
	}

	// get cost of cell
	int cellCost(const Cell& c)
	{
		switch (cells[c.y][c.x])
		{
		case CellState::Lava: return 5;
		case CellState::Water: return 2;
		}
		return 1;
	}

public:
	ChessBoard() 
	{ 
		reset();
		buildMovesGraph();
	}

	// constructs board from source file
	ChessBoard(const std::string& file)
	{
		reset();
		loadFromFile(file);
		buildMovesGraph();
	}

	~ChessBoard()
	{
		for (int y = 0; y < dimY; y++)
			for (int x = 0; x < dimX; x++)
				if (boardGraph[y][x] != nullptr)
				{
					delete boardGraph[y][x];
					boardGraph[y][x] = nullptr;
				}
	}

	// Checks if given path contains valid knight's moves.
	// If we need to print chessboard during moves
	// we save state in each move and print board to console.
	// Returns true if path is valid
	bool MoveKnight(const std::vector<Cell>& path, bool printMoves = false)
	{
		// clear state of board's cells
		reset();

		// empty path is invalid
		if (path.empty())
			return false;

		// path with single pathpoint within chessboard is valid
		if (path.size() == 1)
			if (withinBoard(path[0]))
			{
				if (printMoves)
				{
					setCell(path[0], CellState::Knight);
					std::cout << *this;
				}

				return true;
			}
			else
				return false;

		if (printMoves)
		{
			setCell(path.front(), CellState::Start);
			setCell(path.back(), CellState::End);
		}

		for (std::size_t oi = 0, ti = 1; ti < path.size(); ++ti, ++oi)
		{
			const Cell& oc = path[oi];
			const Cell& tc = path[ti];

			if (!isValidKnightMove(oc, tc))
				return false;

			if (printMoves)
			{
				setCell(oc, oi == 0 ? CellState::Start : CellState::Free);
				setCell(tc, CellState::Knight);
				std::cout << *this;
			}
		}

		return true;
	}

	// Assume origin is valid
	// Level 2
	bool CalculateAnyPath(const Cell& origin, const Cell& target, std::vector<Cell>& path)
	{
		path.clear();
		std::vector<Cell> banned;

		// Level 2
		// Start from origin cell, take it from graph nodes array
		KnightPossibleMoves* m = boardGraph[origin.y][origin.x];

		// while we not reach target cell
		while (m->origin != target)
		{
			bool moveFound = false;

			// in all possible moves from m we find first not visited
			for (const auto& nm : m->possibleMove)
			{
				// here we check if possible move not lead us to visited cell (all visited cells already in path)
				if (std::find(path.begin(), path.end(), nm) == path.end())
				{
					// check if cell not banned
					if (std::find(banned.begin(), banned.end(), nm) == banned.end()) // and hole not banned
					{
						// add found cell to path
						path.push_back(m->origin);
						// next iteration get found cell as origin
						m = boardGraph[nm.y][nm.x];
						// flag setted
						moveFound = true;
						// exit loop
						break;
					}
				}
			}

			// if we not found next move we are in the dead end so...
			if (!moveFound)
			{
				// add this cell to banned cell, to avoid check it again in next iteration
				banned.push_back(m->origin);
				// remove last cell from the path
				path.pop_back();

				if (path.empty())
					return false;

				// take last path cell as origin in the next iteration
				m = boardGraph[path.back().y][path.back().x];
			}
		}

		// add target cell to path
		path.push_back(target);

		return true;
	}

	// Level 3
	bool CalculateShortestPath(const Cell& origin, const Cell& target, std::vector<Cell>& path)
	{
		// Wave algorithm
		path.clear();
		resetGraphNodes();
		KnightPossibleMoves* m = boardGraph[target.y][target.x];
		std::vector<KnightPossibleMoves*> oldFront{ m };
		std::vector<KnightPossibleMoves*> newFront;
		m->waveMark = 0;
		int waveMark = 0;
		bool solutionFound = false;

		while (!solutionFound)
		{
			for (const auto& of : oldFront)
			{
				for (const auto& pm : of->possibleMove)
				{
					KnightPossibleMoves* m = boardGraph[pm.y][pm.x];
					if (m != nullptr && m->waveMark == -1)
					{
						m->waveMark = waveMark + 1;
						newFront.push_back(m);
					}
				}
			}

			if (newFront.empty())
				break;

			if (std::find_if(newFront.begin(), newFront.end(), [origin](const KnightPossibleMoves* const kpm) -> bool{return origin == kpm->origin; }) != newFront.end())
			{
				solutionFound = true;
				break;
			}

			oldFront.swap(newFront);
			newFront.clear();
			waveMark++;
		}

		if (solutionFound)
		{
			// trace back from target to origin
			KnightPossibleMoves *it = boardGraph[origin.y][origin.x];
			waveMark = it->waveMark - 1;
			while (it->origin != target)
			{
				for (const auto& pm : it->possibleMove)
				{
					KnightPossibleMoves* m = boardGraph[pm.y][pm.x];
					if (m->waveMark == waveMark)
					{
						path.push_back(it->origin);
						waveMark--;
						it = m;
					}
				}
			}

			path.push_back(target);
		}

		return solutionFound;
	}

	// Level 4
	// Done with A* algoritm
	bool CalculateShortestPathAStar(const Cell& origin, const Cell& target, std::vector<Cell>& path, bool checkTeleports = true)
	{
		// fast check if origin and target cells not valid
		if (!withinBoard(origin) || !withinBoard(target))
			return false;

		// we can jump from the barrier and rock, but can't reach these cells
		if (cells[target.y][target.x] == CellState::Barrier || cells[target.y][target.x] == CellState::Rock)
			return false;

		resetGraphNodes();

		// First calculate path through teleports
		std::vector<Cell> pathThroughTeleports;

		// check only if only 2 teleports on board
		if (checkTeleports && teleports.size() == 2)
		{
			std::vector<Cell> pathToTeleport0, pathFromTeleport0; // path1 chunks
			std::vector<Cell> pathToTeleport1, pathFromTeleport1; // path2 chunks
			int pathLength1 = 999999, pathLength2 = 999999;

			// get path chunks using 1st teleport -> 2nd teleport move
			CalculateShortestPathAStar(origin, teleports[0], pathToTeleport0, false);
			CalculateShortestPathAStar(teleports[1], target, pathFromTeleport1, false);

			// get path chunks using 2nd teleport -> 1st teleport move
			CalculateShortestPathAStar(origin, teleports[1], pathToTeleport1, false);
			CalculateShortestPathAStar(teleports[0], target, pathFromTeleport0, false);

			// calculate paths length
			// check if 1st path chunks valid
			// if any chunk of path is empty so we can't reach target using teleports
			// condition below guarantees that full path would be valid
			if (!pathToTeleport0.empty() && !pathFromTeleport1.empty())
				pathLength1 = pathToTeleport0.size() + pathFromTeleport1.size();

			if (!pathToTeleport1.empty() && !pathFromTeleport0.empty())
				pathLength2 = pathToTeleport1.size() + pathFromTeleport0.size();

			if (pathLength1 >= pathLength2)
			{
				// we truncate last move from first part cuz step to teleport
				// instantly move knight to destination
				pathThroughTeleports.reserve(pathLength2 - 1);
				pathThroughTeleports.assign(std::begin(pathToTeleport1), std::end(pathToTeleport1) - 1);
				pathThroughTeleports.insert(pathThroughTeleports.end(), std::begin(pathFromTeleport0), std::end(pathFromTeleport0));
			}
			else
			{
				pathThroughTeleports.reserve(pathLength2 - 1);
				pathThroughTeleports.assign(std::begin(pathToTeleport0), std::end(pathToTeleport0) - 1);
				pathThroughTeleports.insert(pathThroughTeleports.end(), std::begin(pathFromTeleport1), std::end(pathFromTeleport1));
			}
		}

		// A*
		path.clear();
		// reverse search to easier path restoration (start search from target cell)
		KnightPossibleMoves* m = boardGraph[target.y][target.x];

		// Add start cell to "open" array which elements need to be checked
		std::vector<KnightPossibleMoves*> open{ m };
		open.reserve(dimX*dimY);
		std::vector<KnightPossibleMoves*> closed;
		bool solutionFound = false;
		KnightPossibleMoves* nicest = nullptr;

		while (!solutionFound)
		{
			// if we out of cells - no solution
			if (open.empty())
				break;

			// get node from "open" with minimal F
			nicest = *(std::min_element(open.begin(), open.end(), [](const KnightPossibleMoves* const e1, const KnightPossibleMoves* const e2) -> bool { return e1->range + e1->h < e2->range + e2->h; }));

			// if we reach origin (we traverse from target to origin)
			if (nicest->origin == origin)
			{
				solutionFound = true;
				break;
			}

			// move node from "open" to "closed" array
			open.erase(std::remove(std::begin(open), std::end(open), nicest));
			closed.push_back(nicest);

			// find and fill next possible nodes reachable from this node
			for (const auto& pm : nicest->possibleMove)
			{
				KnightPossibleMoves* node = boardGraph[pm.y][pm.x];

				if (std::find(std::begin(closed), std::end(closed), node) != std::end(closed))
					continue;

				node->parent = nicest;
				node->range = nicest->range + node->cost;
				node->h = h(node->origin, origin);

				open.push_back(node);
			}
		}

		// build path
		if (solutionFound)
		{
			do
			{
				path.push_back(nicest->origin);
				nicest = nicest->parent;
			} while (nicest->parent != nullptr);

			path.push_back(target);

			// if path through telepots exist and shorter then straight path use path with teleports
			if (!pathThroughTeleports.empty() && pathThroughTeleports.size() < path.size())
				path.swap(pathThroughTeleports);
		}
		else
		{
			if (!pathThroughTeleports.empty())
			{
				path.swap(pathThroughTeleports);
				solutionFound = true;
			}
		}

		return solutionFound;
	}

	template<int dimX, int dimY>
	friend std::ostream& operator << (std::ostream& os, const ChessBoard<dimX, dimY>& board);
};

template<int dimX, int dimY>
std::ostream& operator<<(std::ostream& os, const ChessBoard<dimX, dimY>& board)
{
	os << "Board state:" << std::endl;

	os << "  ";

	for (int x = 0; x < dimX; x++)
		os << std::setw(3) << x;

	os << std::endl;

	for (int y = 0; y < dimY; y++)
	{
		os << std::setw(2) << y;
		for (int x = 0; x < dimX; x++)
			os << std::setw(3) << board.cells[y][x];

		os << std::endl;
	}

	return os;
}

std::ostream& operator<<(std::ostream&, const CellState&);
std::ostream& operator<<(std::ostream&, const Cell&);

template<int dimX, int dimY>
const std::vector<Move> ChessBoard<dimX, dimY>::possibleKnightMoves = {
	{ 1, -2 }, { 2, -1 }, { 2, 1 }, { 1, 2 },
	{ -1, 2 }, { -2, 1 }, { -2, -1 }, { -1, -2 }
};
