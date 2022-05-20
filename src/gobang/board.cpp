#include "board.h"

#include <assert.h>

namespace gobang {

BoardStatus::BoardStatus() {
	reset();
}

bool BoardStatus::drop(int row, int col, ChessType type) {
	assert(row >= 0 && row < GridNumber);
	assert(col >= 0 && col < GridNumber);
	assert(type != ChessType::nil);

	if (m_status[row][col] == ChessType::nil) {
		m_status[row][col] = type;
		m_history.resize(++m_timestamp_ptr);
		m_history.emplace_back(row, col, type);
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
	if ((size_t)m_timestamp_ptr + 1 == m_history.size()) return false;
	const auto [row, col, value] = m_history[++m_timestamp_ptr];
	m_status[row][col] = value;
	return true;
}

bool BoardStatus::judge(int row, int col) const {
	const ChessType target = m_status[row][col];
	if (target == ChessType::nil) return false;

	typedef struct { int x, y; } vec_t;
	vec_t vecs[] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

	for (int i = 0; i < sizeof(vecs) / sizeof(vec_t); ++i) {
		vec_t d = vecs[i], p = {row, col};
		int count = 1;
		while (1) {
			p.x += d.x;
			p.y += d.y;
			if (p.x < 0 && p.x >= GridNumber) break;
			if (p.y < 0 && p.y >= GridNumber) break;
			if (m_status[p.x][p.y] != target) break;
			if (++count == 5) return true;
		}
		d.x = -d.x;
		d.y = -d.y;
		p = {row, col};
		while (1) {
			p.x += d.x;
			p.y += d.y;
			if (p.x < 0 && p.x >= GridNumber) break;
			if (p.y < 0 && p.y >= GridNumber) break;
			if (m_status[p.x][p.y] != target) break;
			if (++count == 5) return true;
		}
	}

	return false;
}

ChessType BoardStatus::get(int row, int col) const {
	return m_status[row][col];
}

bool BoardStatus::check(int row, int col) const {
	return get(row, col) == ChessType::nil;
}

void BoardStatus::reset() {
	for (auto &v : m_status) {
		for (auto &type : v) {
			type = ChessType::nil;
		}
	}
	m_history.clear();
	m_history.reserve(32);
	m_timestamp_ptr = -1;
}

BoardStatus::RecordSet::iterator BoardStatus::drop_begin() {
	return m_history.begin();
}

BoardStatus::RecordSet::iterator BoardStatus::drop_end() {
	return m_history.begin() + m_timestamp_ptr + 1;
}

};
