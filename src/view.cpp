#include "view.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

using namespace gobang;

void View::reset_status() {
	set_origin(7, 7);

	m_origin_x = m_width / 2;
	m_origin_y = m_height / 2;

	m_grid_width = 32;

	m_predrop_row = m_predrop_col = -1;
	m_drop_on_keyctrl = false;

	m_on_drag = false;

	redraw();
}

void View::set_origin(int row, int col) {
	assert(row >= 0 && row < gobang::BoardStatus::GridNumber);
	assert(col >= 0 && col < gobang::BoardStatus::GridNumber);
	POINT origin;
	get_drop(row, col, &origin);
	m_origin_x = origin.x;
	m_origin_y = origin.y;
	m_origin_row = row;
	m_origin_col = col;
}

std::tuple<int, int> View::get_nearest(int x, int y) {
	constexpr size_t N = gobang::BoardStatus::GridNumber;

	RECT board;
	get_board(&board);

	double ix = (x - board.left) * 1.0 / m_grid_width;
	double iy = (y - board.top) * 1.0 / m_grid_width;
	if (ix < 0) ix = 0;
	if (ix >= N - 1) ix = N - 1;
	if (iy < 0) iy = 0;
	if (iy >= N - 1) iy = N - 1;

	int col = ((int)floor(ix * 2) + 1) / 2;
	int row = ((int)floor(iy * 2) + 1) / 2;

	POINT pt;
	get_drop(row, col, &pt);
	if (!PtInRect(&board, pt)) {
		row = col = -1;
	}

	return std::make_tuple(row, col);
}

gobang::Game* View::game() {
	return &m_game;
}

void View::get_board(RECT *rc) {
	rc->left   = m_origin_x - m_origin_col * m_grid_width;
	rc->top    = m_origin_y - m_origin_row * m_grid_width;
	rc->right  = m_origin_x + (gobang::BoardStatus::GridNumber - m_origin_col - 1) * m_grid_width + 1;
	rc->bottom = m_origin_y + (gobang::BoardStatus::GridNumber - m_origin_row - 1) * m_grid_width + 1;
}

void View::get_drop(int row, int col, POINT *pt) {
	assert(row >= 0 && row < gobang::BoardStatus::GridNumber);
	assert(col >= 0 && col < gobang::BoardStatus::GridNumber);
	pt->x = m_origin_x + (col - m_origin_col) * m_grid_width;
	pt->y = m_origin_y + (row - m_origin_row) * m_grid_width;
}

bool View::restart(bool force) {
	if (force) {
		m_game.terminate();
	}
	return m_game.start();
}

void View::join_players() {
	{
		auto user = new ClientUser;
		user->prepared.connect([this]() {
			m_is_user_term = true;
			m_user = dynamic_cast<ClientUser*>(m_black_player);
		});
		m_black_player = user;
		m_game.join(m_black_player, ChessType::black);
	}

	{
		auto tramp = new Tramp;
		tramp->prepared.connect(std::bind(&View::redraw, this, false));
		m_white_player = tramp;
		m_game.join(m_white_player, ChessType::white);
	}

	m_game.dropped.connect([this](int x, int y, ChessType type) {
		redraw();

		if (type == ChessType::black) {
			m_is_user_term = false;
			m_user = nullptr;
		}
		bool succeed = m_game.next_term();
		if (!succeed) {
			m_game.finished();
		}
	});

	m_is_user_term = false;
}

void View::begin_drag(int x, int y) {
	m_drag_start_posx = x;
	m_drag_start_posy = y;
	m_on_drag = true;

	HCURSOR cursor = LoadCursor(NULL, IDC_HAND);
	SetClassLongPtr(m_hwnd, GCLP_HCURSOR, (LONG_PTR)cursor);
}

void View::on_drag(int x, int y) {
	m_origin_x += x - m_drag_start_posx;
	m_origin_y += y - m_drag_start_posy;

	RECT board;
	get_board(&board);
	m_drag_start_posx = x;
	m_drag_start_posy = y;

	redraw();
}

void View::end_drag() {
	m_on_drag = false;

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetClassLongPtr(m_hwnd, GCLP_HCURSOR, (LONG_PTR)cursor);
}

void View::mouse_leave() {
	end_drag();
}

void View::mouse_press(int x, int y, int key_state) {
	begin_drag(x, y);
}

void View::mouse_release(int x, int y, int key_state) {
	end_drag();
}

void View::mouse_move(int x, int y, int key_state) {
	if (m_on_drag) {
		on_drag(x, y);
		return;
	}

	if (!m_is_user_term) return;

	POINT pt;
	auto [row, col] = get_nearest(x, y);
	get_drop(row, col, &pt);
	double dist = sqrt((x - pt.x) * (x - pt.x) + (y - pt.y) * (y - pt.y));
	auto board = m_game.get_status();
	if (board.check(row, col) && dist < 0.36 * m_grid_width) {
		if (row != m_predrop_row || col != m_predrop_col) {
			m_drop_on_keyctrl = false;
			m_predrop_row = row;
			m_predrop_col = col;
			redraw();
		}
	} else if (m_predrop_row != -1 && m_predrop_col != -1
		&& !m_drop_on_keyctrl) {
		m_predrop_row = m_predrop_col = -1;
		redraw();
	}
}

void View::wheel_change(int x, int y, int delta, int key_state) {
	float scalar = delta * 1.0f / WHEEL_DELTA;
	if (scalar > 0.2) scalar = 0.2;
	if (scalar < -0.2) scalar = -0.2;
	scalar += 1;

	m_grid_width = m_grid_width * scalar;
	if (m_grid_width < 24) m_grid_width = 24;
	if (m_grid_width > 256) m_grid_width = 256;

	set_origin(7, 7);
	if (key_state & MK_CONTROL) {
		auto [row, col] = get_nearest(x, y);
		if (row != -1 && col != -1) {
			set_origin(row, col);
		}
	}

	m_preview_cursor->resize(m_grid_width * 0.8, m_grid_width * 0.8);

	redraw();
}

void View::key_down(int key) {
	if (m_predrop_row != -1 && m_predrop_col != -1) {
		switch (key) {
			case VK_LEFT: {
				if (m_predrop_col > 0
					&& game()->get_status().check(m_predrop_row, m_predrop_col - 1)) {
					--m_predrop_col;
				}
			}
			break;
			case VK_UP: {
				if (m_predrop_row > 0
					&& game()->get_status().check(m_predrop_row - 1, m_predrop_col)) {
					--m_predrop_row;
				}
			}
			break;
			case VK_RIGHT: {
				if (m_predrop_col + 1 < BoardStatus::GridNumber
					&& game()->get_status().check(m_predrop_row, m_predrop_col + 1)) {
					++m_predrop_col;
				}
			}
			break;
			case VK_DOWN: {
				if (m_predrop_row + 1 < BoardStatus::GridNumber
					&& game()->get_status().check(m_predrop_row + 1, m_predrop_col)) {
					++m_predrop_row;
				}
			}
			break;
			case VK_RETURN: {
				POINT pt;
				get_drop(m_predrop_row, m_predrop_col, &pt);
				m_drop_on_keyctrl = false;
				clicked(pt.x, pt.y, 0);
			}
			break;
		}
	} else {
		int row = 7, col = 7;
		if (game()->get_status().check(row, col)) {
			m_predrop_row = row;
			m_predrop_col = col;
		} else {
			switch (key) {
				case VK_LEFT: {
					--col;
				}
				break;
				case VK_UP: {
					--row;
				}
				break;
				case VK_RIGHT: {
					++col;
				}
				break;
				case VK_DOWN: {
					++row;
				}
				break;
			}
			if (game()->get_status().check(row, col)) {
				m_predrop_row = row;
				m_predrop_col = col;
			}
		}
	}
	if (m_predrop_row != -1 && m_predrop_col != -1) {
		m_drop_on_keyctrl = true;
	}
	redraw();
}

void View::draw_board(HDC hdc) {
	RECT rc_board = {}, rect = {};
	get_board(&rc_board);
	GetClientRect(m_hwnd, &rect);

	int from_y = rc_board.top >= 0 ? rc_board.top : 0;
	int to_y   = rc_board.bottom <= m_height ? rc_board.bottom : m_height;

	int from_x = rc_board.left >= 0 ? rc_board.left : 0;
	int to_x   = rc_board.right <= m_width ? rc_board.right : m_width;

	if (rc_board.left < 0) {
		rc_board.left += (int)ceil(-rc_board.left * 1.0 / m_grid_width) * m_grid_width;
	}

	if (rc_board.top < 0) {
		rc_board.top += (int)ceil(-rc_board.top * 1.0 / m_grid_width) * m_grid_width;
	}

	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));

	for (int x = rc_board.left; x <= m_width && x <= to_x; x += m_grid_width) {
		MoveToEx(hdc, x, from_y, NULL);
		LineTo(hdc, x, to_y);
	}

	for (int y = rc_board.top; y <= m_height && y <= to_y; y += m_grid_width) {
		MoveToEx(hdc, from_x, y, NULL);
		LineTo(hdc, to_x, y);
	}

	POINT emphasis[] = {{3, 3}, {11, 3}, {7, 7}, {3, 11}, {11, 11}};
	int r = m_grid_width * 1.0 / 9;
	for (int i = 0; i < sizeof(emphasis) / sizeof(POINT); ++i) {
		POINT pt = { };
		get_drop(emphasis[i].x, emphasis[i].y, &pt);
		RECT rdot = { pt.x - r, pt.y - r, pt.x + r, pt.y + r };
		if (PtInRect(&rect, pt)) {
			FillRect(hdc, &rdot, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}
	}

	if (m_is_user_term && m_predrop_col != -1 && m_predrop_row != -1) {
		POINT pt;
		get_drop(m_predrop_row, m_predrop_col, &pt);
		// int r = m_grid_width * 0.32;
		// RECT predrop = { pt.x - r, pt.y - r, pt.x + r, pt.y + r };
		// HBRUSH predrop_brush = CreateSolidBrush(RGB(255, 0, 0));
		// SelectObject(hdc, (HPEN)GetStockObject(NULL_PEN));
		// SelectObject(hdc, predrop_brush);
		// Ellipse(hdc, predrop.left, predrop.top, predrop.right, predrop.bottom);
		// DeleteObject(predrop_brush);
		m_preview_cursor->move(
			pt.x - m_preview_cursor->width() / 2,
			pt.y - m_preview_cursor->height() / 2);
		m_preview_cursor->show(SW_SHOW);
	} else {
		m_preview_cursor->show(SW_HIDE);
	}

	auto board = m_game.get_status();
	for (auto it = board.drop_begin(); it != board.drop_end(); ++it) {
		auto [row, col, type] = *it;
		POINT pt;
		get_drop(row, col, &pt);
		int r = m_grid_width * 0.36;
		RECT drop = { pt.x - r, pt.y - r, pt.x + r, pt.y + r };
		if (type == ChessType::white) {
			SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
		} else {
			SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));
			SelectObject(hdc, (HPEN)GetStockObject(BLACK_PEN));
		}
		Ellipse(hdc, drop.left, drop.top, drop.right, drop.bottom);
	}

	SelectObject(hdc, oldBrush);
}
