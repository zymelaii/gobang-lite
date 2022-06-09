#include "../gobang/gobang.h"

#include <assert.h>

namespace gobang {

Player::Player()
	: m_joined(nullptr) {

}

bool Player::accept(Game *game) {
	if (game == nullptr || m_joined != nullptr) return false;
	m_joined = game;
	return true;
}

ChessType Player::query(int x, int y) {
	assert(m_joined != nullptr);
	return m_joined->get_status().get(x, y);
}

size_t Player::query() {
	assert(m_joined != nullptr);
	return m_joined->get_pieces_num(this);
}

Game::Game()
	: m_white_chess_player(nullptr), m_black_chess_player(nullptr),
	m_on_game(false), m_finished(true), m_enable_forbidden_hand(true) {

}

bool Game::join(Player *player, ChessType type) {
	using namespace std::placeholders;
	if (m_on_game || type == ChessType::nil) return false;
	if (player->accept(this)) {
		if (type == ChessType::white) {
			if (player == m_black_chess_player) return false;
			m_white_chess_player = player;
			player->dropped.connect(std::bind(&Game::drop, this, _1, _2, ChessType::white));
		} else {
			if (player == m_white_chess_player) return false;
			m_black_chess_player = player;
			player->dropped.connect(std::bind(&Game::drop, this, _1, _2, ChessType::black));
		}
		return true;
	}
	return false;
}

bool Game::start() {
	if (!m_white_chess_player || !m_black_chess_player) return false;
	if (!m_on_game || m_on_game && m_finished) {
		m_on_game    = true;
		m_finished   = false;
		m_has_winner = false;

		m_white_pieces_num = 112;
		m_black_pieces_num = 113;

		m_board.reset();

		m_term = ChessType::nil;
		next_term();

		return true;
	}
	return false;
}

void Game::terminate() {
	m_on_game    = false;
	m_finished   = true;
	m_has_winner = false;
}

bool Game::next_term() {
	if (!m_on_game || m_finished) return false;
	if (m_term == ChessType::nil) {
		m_term = ChessType::black;
	} else {
		m_term = m_term == ChessType::white ?
			ChessType::black : ChessType::white;
	}
	size_t pieces_num = m_term == ChessType::white ?
		m_white_pieces_num : m_black_pieces_num;
	if (pieces_num == 0) {
		m_finished   = true;
		m_has_winner = false;
		return false;
	}
	Player *player = m_term == ChessType::white ?
		m_white_chess_player : m_black_chess_player;
	player->prepare();
	return true;
}

ChessType Game::get_winner() const {
	return m_has_winner ? m_term : ChessType::nil;
}

void Game::enable_forbidden_hand(bool state) {
	m_enable_forbidden_hand = state;
}

bool Game::judge_forbidden_hand(int row, int col) {
	int counts[4] = {
		m_board.count_in_direction(row, col, Direction::Horizental),
		m_board.count_in_direction(row, col, Direction::Vertical),
		m_board.count_in_direction(row, col, Direction::LeftOblique),
		m_board.count_in_direction(row, col, Direction::RightOblique),
	};
	int counts_skip_1[4] = {
		m_board.count_in_direction_extend(row, col, Direction::Horizental, 1),
		m_board.count_in_direction_extend(row, col, Direction::Vertical, 1),
		m_board.count_in_direction_extend(row, col, Direction::LeftOblique, 1),
		m_board.count_in_direction_extend(row, col, Direction::RightOblique, 1),
	};
	m_board.undo();
	int counts_skip_1_before[4] = {
		m_board.count_in_direction_extend(row, col, Direction::Horizental, 1),
		m_board.count_in_direction_extend(row, col, Direction::Vertical, 1),
		m_board.count_in_direction_extend(row, col, Direction::LeftOblique, 1),
		m_board.count_in_direction_extend(row, col, Direction::RightOblique, 1),
	};
	m_board.redo();

	for (auto n : counts) {
		if (n == 5) {
			return false;
		}
	}

	for (auto n : counts) {
		if (n > 5) {
			return true;
		}
	}

	printf("skip 1 count {before-drop:[%d,%d,%d,%d], now:[%d,%d,%d,%d]}\n",
		counts_skip_1_before[0], counts_skip_1_before[1],
		counts_skip_1_before[2], counts_skip_1_before[3],
		counts_skip_1[0], counts_skip_1[1], counts_skip_1[2], counts_skip_1[3]);

	for (int i = 0; i < 4; ++i) {
		if (counts_skip_1[i] < 4) continue;
		for (int j = i + 1; j < 4; ++j) {
			if (counts_skip_1[j] < 4) continue;
			if (counts_skip_1_before[i] < 4 && counts_skip_1_before[j] < 4) {
				return true;
			}
		}
	}

	for (int i = 0; i < 4; ++i) {
		if (counts_skip_1[i] < 3) continue;
		for (int j = i + 1; j < 4; ++j) {
			if (counts_skip_1[j] < 3) continue;
			if (counts_skip_1_before[i] < counts_skip_1[i]
				&& counts_skip_1_before[j] < counts_skip_1[j]) {
				return true;
			}
		}
	}

	return false;
}

const BoardStatus& Game::get_status() const {
	return m_board;
}

size_t Game::get_pieces_num(const Player *player) const {
	if (player == m_white_chess_player) {
		return m_white_pieces_num;
	} else if (player == m_black_chess_player) {
		return m_black_pieces_num;
	} else {
		return 0;
	}
}

size_t Game::get_white_pieces_num(const Player *player) const {
	return m_white_pieces_num;
}

size_t Game::get_black_pieces_num(const Player *player) const {
	return m_black_pieces_num;
}

ChessType Game::get_chess_type(const Player *player) const {
	if (player == nullptr) return ChessType::nil;
	if (player == m_white_chess_player) return ChessType::white;
	if (player == m_black_chess_player) return ChessType::black;
	return ChessType::nil;
}

void Game::drop(int row, int col, ChessType type) {
	if (type == ChessType::white) {
		if (m_white_pieces_num > 0) {
			--m_white_pieces_num;
		} else {
			return;
		}
	} else if (type == ChessType::black) {
		if (m_black_pieces_num > 0) {
			--m_black_pieces_num;
		} else {
			return;
		}
	} else {
		return;
	}
	bool succeed = m_board.drop(row, col, type);
	if (succeed && type == ChessType::black) {
		if (judge_forbidden_hand(row, col)) {
			succeed = false;
			m_board.undo();
			caught_forbidden_hand(row, col);
		}
	}
	if (succeed) {
		if (m_board.judge(row, col)) {
			m_finished   = true;
			m_has_winner = true;
		}
		dropped(row, col, type);
	}
}

};
