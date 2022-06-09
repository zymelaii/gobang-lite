#ifndef PLAYER_AIV1_H
#define PLAYER_AIV1_H

#include "../ui/prototype.h"
#include "../gobang/gobang.h"
#include <tuple>

using namespace gobang;

class AIv1 final : public Player {
	constexpr static size_t N = BoardStatus::GridNumber;
	constexpr static ChessType m_self = ChessType::white;
	constexpr static ChessType m_other = ChessType::black;
public:
	AIv1();
protected:
	std::tuple<int,int> get_prev_drop();
	void cost_drop(int row, int col);
public:
	void prepare() override;
public:
	ui::UiObject::signal<void()> prepared;
private:
	ChessType m_board[N][N];
	double m_weight[N][N];
};

#endif /*PLAYER_AIV1_H*/
