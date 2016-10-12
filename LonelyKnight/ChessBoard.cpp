#include "ChessBoard.h"

std::ostream& operator<<(std::ostream& os, const CellState& state)
{
	switch (state)
	{
	case CellState::Free: return os << '.';
	case CellState::Start: return os << 'S';
	case CellState::End: return os << 'E';
	case CellState::Knight: return os << 'K';
	case CellState::Barrier: return os << 'B';
	case CellState::Lava: return os << 'L';
	case CellState::Rock: return os << 'R';
	case CellState::Teleport: return os << 'T';
	case CellState::Water: return os << 'W';
	case CellState::PathPoint: return os << '*';
	}

	return os << '?';
}

std::ostream& operator<<(std::ostream& os, const Cell& cell)
{
	return os << "[" << std::setw(2) << cell.x << std::setw(0) << ", " << std::setw(2) << cell.y << std::setw(0) << "]";
}
