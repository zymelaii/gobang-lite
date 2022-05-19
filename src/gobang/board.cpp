#include "board.h"

#include <assert.h>

namespace gobang {

BoardStatus::BoardStatus()
	: m_status(), m_timestamp_ptr(-1) {
	m_history.clear();
	m_history.reserve(32);
}

bool BoardStatus::drop(int x, int y, ChessType type) {
	assert(x >= 0 && x < GridNumber);
	assert(y >= 0 && y < GridNumber);
	assert(type != ChessType::nil);

	if (m_status[x][y] == ChessType::nil) {
		m_status[x][y] = type;
		m_history.resize(++m_timestamp_ptr);
		m_history.emplace_back(x, y, type);
		return true;
	}

	return false;
}

bool BoardStatus::undo() {
	if (m_timestamp_ptr < 0) return false;
	const auto [row, col, value] = m_history[m_timestamp_ptr--];
	m_status[row][col] = ChessType::nil;
	return true;
}

bool BoardStatus::redo() {
	if (m_timestamp_ptr + 1 == m_history.size()) return false;
	const auto [row, col, value] = m_history[++m_timestamp_ptr];
	m_status[row][col] = value;
	return true;
}

bool BoardStatus::judge(int x, int y) const {
	return false;
}

};
