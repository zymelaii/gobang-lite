#include "../gobang/gobang.h"

#include <assert.h>

namespace gobang {

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
	m_on_game(false) {

}

bool Game::join(Player *player, ChessType type) {
	using namespace std::placeholders;
	if (m_on_game || type == ChessType::nil) return false;
	if (player->accept(this)) {
		if (type == ChessType::white) {
			if (player == m_white_chess_player) return true;
			if (player == m_black_chess_player) return false;
			player->connect(std::bind(&Game::drop, this, _1, _2, ChessType::white));
		} else {
			if (player == m_white_chess_player) return false;
			if (player == m_black_chess_player) return true;
			player->connect(std::bind(&Game::drop, this, _1, _2, ChessType::black));
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

		m_term = ChessType::black;

		m_white_pieces_num = 112;
		m_black_pieces_num = 113;

		m_board.reset();
		return true;
	}
	return false;
}

bool Game::finish() const {
	return m_finished;
}

bool Game::next_term() {
	if (!m_on_game || m_finished) return false;
	m_term = m_term == ChessType::white ?
		ChessType::black : ChessType::white;
	size_t pieces_num = m_term == ChessType::white ?
		m_white_pieces_num : m_black_pieces_num;
	if (pieces_num == 0) {
		m_finished = true;
		m_has_winner = false;
		return false;
	}
	return true;
}

ChessType Game::get_winner() const {
	return m_has_winner ? ChessType::nil : m_term;
}

const BoardStatus& Game::get_status() const {
	return m_board;
}

size_t Game::get_pieces_num(Player *player) const {
	if (player == m_white_chess_player) {
		return m_white_pieces_num;
	} else if (player == m_black_chess_player) {
		return m_black_pieces_num;
	} else {
		return 0;
	}
}

size_t Game::get_white_pieces_num(Player *player) const {
	return m_white_pieces_num;
}

size_t Game::get_black_pieces_num(Player *player) const {
	return m_black_pieces_num;
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
	m_board.drop(row, col, type);
}

};
