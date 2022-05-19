#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <tuple>

namespace gobang {

enum class ChessType { black, white, nil };

class BoardStatus {
public:
	static const size_t GridNumber = 15;
public:
	BoardStatus();
	/**
	 * @breif dorp a chess
	 */
	bool drop(int x, int y, ChessType type);
	/**
	 * @breif undo chess-drop
	 */
	bool undo();
	/**
	 * @breif redo chess-drop-undo
	 */
	bool redo();
	/**
	 * @breif judge win-condition at (x,y)
	 */
	bool judge(int x, int y) const;
private:
	ChessType m_status[GridNumber][GridNumber];

	std::vector<std::tuple<int, int, ChessType>> m_history;
	int m_timestamp_ptr;
};

};

#endif /*BOARD_H*/
