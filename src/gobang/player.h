#ifndef PLAYER_H
#define PLAYER_H

#include "gobang/board.h"
#include "../ui/prototype.h"

#include <functional>
#include <vector>

namespace gobang {

class PlayerBase {
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
public:
	ui::UiObject::signal<void(int,int)> dropped;
};

};

#endif /*PLAYER_H*/
