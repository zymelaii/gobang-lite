#ifndef PLAYER_H
#define PLAYER_H

#include "gobang/board.h"

#include <functional>
#include <vector>

namespace gobang {

class PlayerBase {
public:
	using DroppedSlot = std::function<void(int,int)>;
public:
	/**
	 * @breif notify the player to prepare an action
	 */
	virtual void prepare() = 0;

	/**
	 * @breif query the drop status of the chessboard
	 */
	virtual ChessType query(int x, int y) = 0;

	/**
	 * @breif query the number of remaining pieces
	 */
	virtual size_t query() = 0;

	/**
	 * @breif inform the player to prepare to drop
	 */
	bool drop(int x, int y);

	void connect(DroppedSlot slot);

	template <typename F, typename T>
	void connect(F &&slot, T *resolver) {
		using namespace std::placeholders;
		connect(std::bind(slot, resolver, _1, _2));
	}
private:
	std::vector<DroppedSlot> m_dropped_slots;
};

};

#endif /*PLAYER_H*/
