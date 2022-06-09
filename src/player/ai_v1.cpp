#include "ai_v1.h"
#include <assert.h>
#include <future>
#include <thread>
#include <windows.h>

AIv1::AIv1() {
	for (int row = 0; row < N; ++row) {
		std::fill(m_board[row], m_board[row] + N, ChessType::nil);
		std::fill(m_weight[row], m_weight[row] + N, 0.0);
	}
}

std::tuple<int,int> AIv1::get_prev_drop() {
	for (int row = 0; row < N; ++row) {
		for (int col = 0; col < N; ++col) {
			if (query(row, col) != m_board[row][col]) {
				return std::make_tuple(row, col);
			}
		}
	}
	return std::make_tuple(-1, -1);
}

void AIv1::cost_drop(int row, int col) {
	auto type = query(row, col);
	double ks[3] = {2.0, 1.5, 1.2};
	double ko[3] = {1.0/ks[0], 1.0/ks[1], 1.0/ks[2]};
	if (type == m_self) {
		if (m_weight[row][col] == 0.0) m_weight[row][col] = 1.0;
		m_weight[row][col] *=  ks[0];
		for (int i = 1; i < 3; ++i) {
			if (col - i >= 0) m_weight[row][col - i] *= ks[i];
			if (col + i < N) m_weight[row][col + i] *= ks[i];
			if (row - i >= 0) m_weight[row - i][col] *= ks[i];
			if (row + i < N) m_weight[row + i][col] *= ks[i];
		}
	} else {
		if (m_weight[row][col] == 0.0) m_weight[row][col] = 1.0;
		m_weight[row][col] *=  ko[0];
		for (int i = 1; i < 3; ++i) {
			if (col - i >= 0) m_weight[row][col - i] *= 1 + ko[i];
			if (col + i < N) m_weight[row][col + i] *= 1 + ko[i];
			if (row - i >= 0) m_weight[row - i][col] *= 1 + ko[i];
			if (row + i < N) m_weight[row + i][col] *= 1 + ko[i];
		}
	}
	m_board[row][col] = type;
}

void AIv1::prepare() {
	auto [row, col] = get_prev_drop();
	cost_drop(row, col);
	int pos = 0;
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int row = pos / N, col = pos % N;
			if (m_board[row][col] != ChessType::nil) {
				pos = i * N + j;
			} else if (m_weight[i][j] > m_weight[row][col]) {
				pos = i * N + j;
			}
		}
	}
	row = pos / N;
	col = pos % N;
	std::packaged_task<void()> task([this, row, col]() {
		Sleep(rand() % 3000 + 600);
		drop(row, col);
		cost_drop(row, col);
		prepared();
	});
    std::thread(std::move(task)).detach();
}
