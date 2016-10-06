#pragma once

#include <vector>
#include <iostream>

enum class CellState : unsigned char { Free, Start, End, Knight };

struct Cell
{
	int x;
	int y;
};

template <int dimX = 8, int dimY = 8> class ChessBoard
{
private:
	CellState cells[dimX][dimY];

	// Checks if given coordinates belongs to chess board
	bool withinBoard(const Cell& c) const { return c.x >= 0 && c.y >= 0 && c.x < dimX && c.y < dimY; }

	// Check if given origin and target cells are valid kinight's move
	bool isValidKnightMove(const Cell& o, const Cell& t) const { return (o.x - t.x) * (o.x - t.x) + (o.y - t.y) * (o.y - t.y) == 5; }

	// Set particular cells to given state
	void setCell(const Cell& c, CellState state) { if (withinBoard(c)) cells[c.x][c.y] = state; }

	void reset() { memset(cells, static_cast<int>(CellState::Free), sizeof(CellState) * dimX * dimY); }

public:
	ChessBoard() { reset(); }

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
