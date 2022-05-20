#ifndef GOBANG_H
#define GOBANG_H

#include "../gobang/board.h"
#include "../gobang/player.h"

namespace gobang {

class Game;

class Player : public PlayerBase {
public:
	Player();
	virtual bool accept(Game *game);
	ChessType query(int x, int y) override;
	size_t query() override;
private:
	Game *m_joined;
};

class Game {
public:
	Game();
	bool join(Player *player, ChessType type);
	bool start();
	void terminate();
	bool finished() const;
	bool next_term();
	ChessType get_winner() const;
public:
	const BoardStatus& get_status() const;
	size_t get_pieces_num(const Player *player) const;
	size_t get_white_pieces_num(const Player *player) const;
	size_t get_black_pieces_num(const Player *player) const;
	ChessType get_chess_type(const Player *player) const;

	using DroppedSlot = std::function<void(int,int,ChessType)>;
	void connect(DroppedSlot slot);
protected:
	void drop(int row, int col, ChessType type);
	void dropped(int row, int col, ChessType type);
private:
	BoardStatus m_board;

	Player *m_white_chess_player;
	Player *m_black_chess_player;

	size_t m_white_pieces_num;
	size_t m_black_pieces_num;

	ChessType m_term;

	bool m_on_game;
	bool m_finished;
	bool m_has_winner;

	std::vector<DroppedSlot> m_dropped_slots;
};

};

#endif /*GOBANG_H*/
