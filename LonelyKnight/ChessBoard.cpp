#include "ChessBoard.h"

std::ostream& operator<<(std::ostream& os, const CellState& state)
{
	switch (state)
	{
	case CellState::Free: return os << '.';
	case CellState::Start: return os << 'S';
	case CellState::End: return os << 'E';
	case CellState::Knight: return os << 'K';
	}

	return os << '?';
}

//template<typename T> using v_iter = std::vector<T>::iterator;

