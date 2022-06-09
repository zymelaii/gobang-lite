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

int BoardStatus::count_in_direction(int row, int col, Direction d) const {
	const ChessType target = m_status[row][col];
	if (target == ChessType::nil) return 0;
	typedef struct { int x, y; } vec_t;
	vec_t vec = { }, p = {row, col};
	switch (d) {
		case Direction::Horizental: vec = { 1, 0 }; break;
		case Direction::Vertical: vec = { 0, 1 }; break;
		case Direction::LeftOblique: vec = { 1, -1 }; break;
		case Direction::RightOblique: vec = { 1, 1 }; break;
	}
	int count = 1;
	while (1) {
		p.x += vec.x;
		p.y += vec.y;
		if (p.x < 0 && p.x >= GridNumber) break;
		if (p.y < 0 && p.y >= GridNumber) break;
		if (m_status[p.x][p.y] != target) break;
		++count;
	}
	vec.x = -vec.x;
	vec.y = -vec.y;
	p = {row, col};
	while (1) {
		p.x += vec.x;
		p.y += vec.y;
		if (p.x < 0 && p.x >= GridNumber) break;
		if (p.y < 0 && p.y >= GridNumber) break;
		if (m_status[p.x][p.y] != target) break;
		++count;
	}
	return count;
}

int BoardStatus::count_in_direction_extend(int row, int col, Direction d, int n) const {
	ChessType target = m_status[row][col];
	if (target == ChessType::nil) target = ChessType::black;
	typedef struct { int x, y; } vec_t;
	vec_t vec = { }, p = {row, col};
	switch (d) {
		case Direction::Horizental: vec = { 1, 0 }; break;
		case Direction::Vertical: vec = { 0, 1 }; break;
		case Direction::LeftOblique: vec = { 1, -1 }; break;
		case Direction::RightOblique: vec = { 1, 1 }; break;
	}
	int count = m_status[row][col] == ChessType::nil ? 0 : 1;
	int remain_blanks = n, handled_blanks = 0;
	while (1) {
		p.x += vec.x;
		p.y += vec.y;
		if (p.x < 0 && p.x >= GridNumber) break;
		if (p.y < 0 && p.y >= GridNumber) break;
		if (remain_blanks > 0 && m_status[p.x][p.y] == ChessType::nil) {
			--remain_blanks;
			++handled_blanks;
			continue;
		}
		if (m_status[p.x][p.y] != target) break;
		++count;
		handled_blanks = 0;
	}
	remain_blanks += handled_blanks;
	vec.x = -vec.x;
	vec.y = -vec.y;
	p = {row, col};
	while (1) {
		p.x += vec.x;
		p.y += vec.y;
		if (p.x < 0 && p.x >= GridNumber) break;
		if (p.y < 0 && p.y >= GridNumber) break;
		if (remain_blanks > 0 && m_status[p.x][p.y] == ChessType::nil) {
			--remain_blanks;
			++handled_blanks;
			continue;
		}
		if (m_status[p.x][p.y] != target) break;
		++count;
		handled_blanks = 0;
	}
	remain_blanks += handled_blanks;
	return count;
}

bool BoardStatus::judge(int row, int col) const {
	const ChessType target = m_status[row][col];
	if (target == ChessType::nil) return false;

	if (count_in_direction(row, col, Direction::Horizental) >= 5) return true;
	if (count_in_direction(row, col, Direction::Vertical) >= 5) return true;
	if (count_in_direction(row, col, Direction::LeftOblique) >= 5) return true;
	if (count_in_direction(row, col, Direction::RightOblique) >= 5) return true;

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
