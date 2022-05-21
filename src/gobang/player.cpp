#include "../gobang/player.h"

#include <stdio.h>
namespace gobang {

bool PlayerBase::drop(int x, int y) {
	if (query(x, y) != ChessType::nil) return false;
	dropped(x, y);
	return true;
}

};
