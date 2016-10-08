#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <array>

enum class CellState : unsigned char { Free, Start, End, Knight };

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

template <int dimX = 8, int dimY = 8> class ChessBoard
{
private:
	// Class represents a graph's node
	class KnightPossibleMoves
	{
	private:
		// possible moves of knigh't as offsets from position
		const static std::vector<Move> possibleMoves;
		//const static std::vector<Cell> visitedCells;

		//Cell applyMove(const Cell& origin, const Move& move) { return { origin.x + move.dx, origin.y + move.dy }; }

	public:
		// for waves algorithm
		int waveMark;
		// node's coordinates on board
		Cell origin;
		// adjacent nodes for this node
		std::vector<KnightPossibleMoves*> nextMoves;

		KnightPossibleMoves(const Cell& position)
			: origin(position), waveMark(-1) {}

		// Get array of cells in which knight can move in one turn
		std::vector<Cell> GetValidMoves()
		{
			// Knight have maximum 8 moves
			std::vector<Cell> targets(8);

			// apply possible moves to origin to obtain adjacent cells
			std::transform(
				possibleMoves.begin(), possibleMoves.end(),
				targets.begin(),
				[this](Move m) -> Cell { return{ origin.x + m.dx, origin.y + m.dy }; });

			// remove cells which are out of board
			targets.erase(
				std::remove_if(targets.begin(), targets.end(),
				[](Cell c) -> bool { return !(c.x >= 0 && c.y >= 0 && c.x < dimX && c.y < dimY); }),
				targets.end());

			return targets;
		}

		// Adds adjacent node
		void AddNextMove(KnightPossibleMoves* pm)
		{
			nextMoves.push_back(pm);
		}
	};

	// Board's cells states array
	CellState cells[dimX][dimY];

	// Board's graph for knight moves
	KnightPossibleMoves *boardGraph[dimX][dimY];

	// Checks if given coordinates belongs to chess board
	bool withinBoard(const Cell& c) const { return c.x >= 0 && c.y >= 0 && c.x < dimX && c.y < dimY; }

	// Check if given origin and target cells are valid knight's move
	bool isValidKnightMove(const Cell& o, const Cell& t) const { return (o.x - t.x) * (o.x - t.x) + (o.y - t.y) * (o.y - t.y) == 5; }

	// Set particular cells to given state
	void setCell(const Cell& c, CellState state) { if (withinBoard(c)) cells[c.x][c.y] = state; }

	// Resets board to initial state
	void reset() { memset(cells, static_cast<int>(CellState::Free), sizeof(CellState) * dimX * dimY); }

	// Build knight's moves graphs
	void buildMovesGraph()
	{
		// Init board.
		// Board have dimX * dimY graph nodes
		memset(boardGraph, 0, sizeof(KnightPossibleMoves*) * dimX * dimY);

		// Init array of moves which need to be processed in next iteration
		std::vector<KnightPossibleMoves*> nextMoves;

		// Reserve space to avoid reallocations so all iterators stay valid
		nextMoves.reserve(dimX*dimY);

		// Get first cell, so first iteration find adjacent moves for it
		// next iteration find adjacent cells for adjacent cells for first cell
		// and so on
		nextMoves.push_back(new KnightPossibleMoves(Cell{0, 0}));

		for (auto moveIt = nextMoves.begin(); moveIt != nextMoves.end(); moveIt++)
		{
			KnightPossibleMoves* m = *moveIt;

			// If graph node not yet been created for this cell
			if (boardGraph[m->origin.x][m->origin.y] == nullptr)
				boardGraph[m->origin.x][m->origin.y] = m;

			// Get valid knight's moves from cell
			std::vector<Cell> moves = m->GetValidMoves();

			// for each found possible move
			for (auto pm : moves)
			{
				// If no graph node ceated for "possible move" create it, otherwise take existing
				KnightPossibleMoves *nm = boardGraph[pm.x][pm.y] == nullptr ? new KnightPossibleMoves{ pm } : boardGraph[pm.x][pm.y];

				// If no node stored in graph for this move
				// store it and add it to nextMoves array to include it for next iterations
				if (boardGraph[pm.x][pm.y] == nullptr)
				{
					nextMoves.push_back(nm);
					boardGraph[pm.x][pm.y] = nm;
				}

				//
				// Here add child node to node
				m->AddNextMove(nm);
			}
		}
	}

	void resetWaveMarks()
	{
		for (int x = 0; x < dimX; x++)
			for (int y = 0; y < dimY; y++)
				boardGraph[x][y]->waveMark = -1;
	}

public:
	ChessBoard() 
	{ 
		reset();
		buildMovesGraph();
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

			// check if cell within chessboard
			if (!withinBoard(oc))
				return false;

			const Cell& tc = path[ti];

			// check if cell within chessboard
			if (!withinBoard(tc))
				return false;

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
		KnightPossibleMoves* m = boardGraph[origin.x][origin.y];

		// while we not reach target cell
		while (m->origin != target)
		{
			bool moveFound = false;

			// in all possible moves from m we find first not visited
			for (auto nm : m->nextMoves)
			{
				// here we check if possible move not lead us to visited cell (all visited cells already in path)
				if (std::find(path.begin(), path.end(), nm->origin) == path.end())
				{
					// check if cell not banned
					if (std::find(banned.begin(), banned.end(), nm->origin) == banned.end()) // and hole not banned
					{
						// add found cell to path
						path.push_back(m->origin);
						// next iteration get found cell as origin
						m = nm;
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
				// take last path cell as origin in the next iteration
				m = boardGraph[path.back().x][path.back().y];
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
		resetWaveMarks();
		KnightPossibleMoves* m = boardGraph[origin.x][origin.y];
		std::vector<KnightPossibleMoves*> oldFront{ m };
		std::vector<KnightPossibleMoves*> newFront;
		m->waveMark = 0;
		int waveMark = 0;
		bool solutionFound = false;

		while (!solutionFound)
		{
			for (auto of : oldFront)
			{
				for (auto pm : of->nextMoves)
				{
					if (pm->waveMark == -1)
					{
						pm->waveMark = waveMark + 1;
						newFront.push_back(pm);
					}
				}
			}

			if (newFront.empty())
				break;

			if (std::find_if(newFront.begin(), newFront.end(), [target](KnightPossibleMoves* kpm) -> bool{return target == kpm->origin; }) != newFront.end())
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
			KnightPossibleMoves *it = boardGraph[target.x][target.y];
			waveMark = it->waveMark - 1;
			while (it->origin != origin)
			{
				for (auto n : it->nextMoves)
					if (n->waveMark == waveMark)
					{
						path.push_back(it->origin);
						waveMark--;
						it = n;
					}
			}

			path.push_back(origin);
		}

		std::reverse(path.begin(), path.end());

		return solutionFound;
	}

	template<int dimX, int dimY>
	friend std::ostream& operator << (std::ostream& os, const ChessBoard<dimX, dimY>& board);
};

template<int dimX, int dimY>
std::ostream& operator<<(std::ostream& os, const ChessBoard<dimX, dimY>& board)
{
	os << "Board state:" << std::endl;

	for (int y = 0; y < dimY; y++)
	{
		for (int x = 0; x < dimX; x++)
			os << board.cells[x][y] << ' ';

		os << std::endl;
	}

	return os;
}

std::ostream& operator<<(std::ostream&, const CellState&);

template<int dimX, int dimY>
const std::vector<Move> ChessBoard<dimX, dimY>::KnightPossibleMoves::possibleMoves = {
	{ 1, -2 }, { 2, -1 }, { 2, 1 }, { 1, 2 },
	{ -1, 2 }, { -2, 1 }, { -2, -1 }, { -1, -2 }
};
