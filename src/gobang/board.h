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
	bool drop(int row, int col, ChessType type);
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
	bool judge(int row, int col) const;
	/**
	 * @brief get dropped status
	 */
	ChessType get(int row, int col) const;
	/**
	 * @brief check whether drop is available
	 */
	bool check(int row, int col) const;
	/**
	 * @brief reset board
	 */
	void reset();
public:
	using RecordSet = std::vector<std::tuple<int, int, ChessType>>;
	RecordSet::iterator drop_begin();
	RecordSet::iterator drop_end();
private:
	ChessType m_status[GridNumber][GridNumber];

	RecordSet m_history;
	int m_timestamp_ptr;
};

};

#endif /*BOARD_H*/
