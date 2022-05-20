#include "../gobang/player.h"

#include <stdio.h>
namespace gobang {

bool PlayerBase::drop(int x, int y) {
	if (query(x, y) != ChessType::nil) return false;
	for (auto &solve : m_dropped_slots) {
		solve(x, y);
	}
	return true;
}

void PlayerBase::connect(DroppedSlot slot) {
	m_dropped_slots.emplace_back(std::move(slot));
}

};
