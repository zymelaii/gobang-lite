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

	m_predrop_row = -1;
	m_predrop_col = -1;
}

void View::set_origin(int row, int col) {
	assert(row >= 0 && row < m_bs.GridNumber);
	assert(col >= 0 && col < m_bs.GridNumber);
	m_origin_row = row;
	m_origin_col = col;
}

std::tuple<int, int> View::get_nearest(int x, int y) {
	double dist = 1e6;
	int row = 0, col = 0;
	for (int i = 0; i < m_bs.GridNumber; ++i) {
		for (int j = 0; j < m_bs.GridNumber; ++j) {
			POINT pt;
			get_drop(i, j, &pt);
			double t = (pt.x - x) * (pt.x - x) + (pt.y - y) * (pt.y - y);
			if (t < dist) {
				dist = t;
				row = i;
				col = j;
			}
		}
	}
	return std::make_tuple(row, col);
}

void View::get_board(RECT *rc) {
	rc->left   = m_origin_x - m_origin_col * m_grid_width;
	rc->top    = m_origin_y - m_origin_row * m_grid_width;
	rc->right  = m_origin_x + (m_bs.GridNumber - m_origin_col - 1) * m_grid_width;
	rc->bottom = m_origin_y + (m_bs.GridNumber - m_origin_row - 1) * m_grid_width;
}

void View::get_drop(int row, int col, POINT *pt) {
	assert(row >= 0 && row < m_bs.GridNumber);
	assert(col >= 0 && col < m_bs.GridNumber);
	pt->x = m_origin_x + (col - m_origin_col) * m_grid_width;
	pt->y = m_origin_y + (row - m_origin_row) * m_grid_width;
}

void View::begin_drag(int x, int y) {
	m_drag_start_posx = x;
	m_drag_start_posy = y;
	m_on_drag = true;

	TRACKMOUSEEVENT track = { };
	track.hwndTrack   = m_hwnd;
	track.dwFlags     = TME_LEAVE;
	track.dwHoverTime = HOVER_DEFAULT;
	track.cbSize      = sizeof(TRACKMOUSEEVENT);
	TrackMouseEvent(&track);

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

	InvalidateRect(m_hwnd, NULL, FALSE);
}

void View::end_drag() {
	m_on_drag = false;

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetClassLongPtr(m_hwnd, GCLP_HCURSOR, (LONG_PTR)cursor);
}

void View::mouse_leave() {
	end_drag();
}

void View::created() {
	reset_status();
}

void View::resized(int width, int height, int type) {
	reset_status();
	InvalidateRect(m_hwnd, NULL, FALSE);
}

void View::double_click(int x, int y, int key_state) {
	reset_status();
	InvalidateRect(m_hwnd, NULL, FALSE);
}

void View::mouse_press(int x, int y, int key_state) {
	begin_drag(x, y);

	if (m_predrop_row != -1 && m_predrop_col != -1) {
		static bool term = true;
		ChessType type = term ? ChessType::white : ChessType::black;
		bool succeed = m_bs.drop(m_predrop_row, m_predrop_col, type);
		if (succeed) {
			InvalidateRect(m_hwnd, NULL, FALSE);
			if (m_bs.judge(m_predrop_row, m_predrop_col)) {
				MessageBox(m_hwnd,
					"是的，不管怎么样都是你赢了，\n"
					"毕竟只有你在下。\n"
					"现在，你可以选择退出或继续和BUG玩耍。",
					"来自zymelaii的提示", MB_OK);
			}
			m_predrop_row = -1;
			m_predrop_col = -1;
			term = !term;
		}
	}
}

void View::mouse_release(int x, int y, int key_state) {
	end_drag();
}

void View::mouse_move(int x, int y, int key_state) {
	if (!m_on_drag) {
		POINT pt;
		auto [row, col] = get_nearest(x, y);
		get_drop(row, col, &pt);
		double dist = sqrt((x - pt.x) * (x - pt.x) + (y - pt.y) * (y - pt.y));
		if (m_bs.check(row, col) && dist < 0.36 * m_grid_width) {
			if (row != m_predrop_row || col != m_predrop_col) {
				m_predrop_row = row;
				m_predrop_col = col;
				InvalidateRect(m_hwnd, NULL, FALSE);
			}
		} else {
			m_predrop_row = -1;
			m_predrop_col = -1;
			InvalidateRect(m_hwnd, NULL, FALSE);
		}
	} else {
		on_drag(x, y);
	}
}

void View::wheel_change(int x, int y, int delta) {
	float scalar = delta * 1.0f / WHEEL_DELTA;
	if (scalar > 0.2) scalar = 0.2;
	if (scalar < -0.2) scalar = -0.2;
	scalar += 1;

	// double kdx = (m_origin_x - x) * 1.0 / m_grid_width;
	// double kdy = (m_origin_y - y) * 1.0 / m_grid_width;

	m_grid_width = m_grid_width * scalar;
	if (m_grid_width < 24) m_grid_width = 24;
	if (m_grid_width > 256) m_grid_width = 256;

	// m_origin_x = x + (int)(kdx * m_grid_width);
	// m_origin_y = y + (int)(kdy * m_grid_width);

	InvalidateRect(m_hwnd, NULL, FALSE);
}

void View::render() {
	RECT board = {}, rect = {};
	get_board(&board);
	GetClientRect(m_hwnd, &rect);

	int from_y = board.top >= 0 ? board.top : 0;
	int to_y   = board.bottom <= m_height ? board.bottom : m_height;

	int from_x = board.left >= 0 ? board.left : 0;
	int to_x   = board.right <= m_width ? board.right : m_width;

	if (board.left < 0) {
		board.left += (int)ceil(-board.left * 1.0 / m_grid_width) * m_grid_width;
	}

	if (board.top < 0) {
		board.top += (int)ceil(-board.top * 1.0 / m_grid_width) * m_grid_width;
	}

	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	HDC hmdc = CreateCompatibleDC(ps.hdc);
	HBITMAP hbitmap = CreateCompatibleBitmap(ps.hdc, m_width, m_height);
	SelectObject(hmdc, hbitmap);
	SelectObject(hmdc, (HBRUSH)GetStockObject(BLACK_BRUSH));

	HBRUSH wooden_brush = CreateSolidBrush(RGB(251, 228, 152));
	FillRect(hmdc, &rect, wooden_brush);
	DeleteObject(wooden_brush);

	for (int x = board.left; x <= m_width && x <= to_x; x += m_grid_width) {
		MoveToEx(hmdc, x, from_y, NULL);
		LineTo(hmdc, x, to_y);
	}

	for (int y = board.top; y <= m_height && y <= to_y; y += m_grid_width) {
		MoveToEx(hmdc, from_x, y, NULL);
		LineTo(hmdc, to_x, y);
	}

	POINT emphasis[] = {{3, 3}, {11, 3}, {7, 7}, {3, 11}, {11, 11}};
	int r = m_grid_width * 1.0 / 9;
	for (int i = 0; i < sizeof(emphasis) / sizeof(POINT); ++i) {
		POINT pt = { };
		get_drop(emphasis[i].x, emphasis[i].y, &pt);
		RECT rdot = { pt.x - r, pt.y - r, pt.x + r, pt.y + r };
		if (PtInRect(&rect, pt)) {
			FillRect(hmdc, &rdot, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}
	}

	if (m_predrop_col != -1 && m_predrop_row != -1) {
		POINT pt;
		get_drop(m_predrop_row, m_predrop_col, &pt);
		int r = m_grid_width * 0.32;
		RECT predrop = { pt.x - r, pt.y - r, pt.x + r, pt.y + r };
		HBRUSH predrop_brush = CreateSolidBrush(RGB(255, 0, 0));
		SelectObject(hmdc, (HPEN)GetStockObject(NULL_PEN));
		SelectObject(hmdc, predrop_brush);
		Ellipse(hmdc, predrop.left, predrop.top, predrop.right, predrop.bottom);
		DeleteObject(predrop_brush);
	}

	for (auto it = m_bs.drop_begin(); it != m_bs.drop_end(); ++it) {
		auto [row, col, type] = *it;
		POINT pt;
		get_drop(row, col, &pt);
		int r = m_grid_width * 0.36;
		RECT drop = { pt.x - r, pt.y - r, pt.x + r, pt.y + r };
		if (type == ChessType::white) {
			SelectObject(hmdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			SelectObject(hmdc, (HPEN)GetStockObject(WHITE_PEN));
		} else {
			SelectObject(hmdc, (HBRUSH)GetStockObject(BLACK_BRUSH));
			SelectObject(hmdc, (HPEN)GetStockObject(BLACK_PEN));
		}
		Ellipse(hmdc, drop.left, drop.top, drop.right, drop.bottom);
	}

	BitBlt(ps.hdc, 0, 0, m_width, m_height, hmdc, 0, 0, SRCCOPY);

	DeleteDC(hmdc);
	DeleteObject(hbitmap);

	EndPaint(m_hwnd, &ps);
}
