#include "ui/widget.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

using gobang::ui::Widget;
using gobang::ui::UiObject;

class View : public Widget {
	UI_OBJECT;
public:
	View() {
		UiObject::reset(__class__());
	}
	View(int width, int height)
		: Widget(width, height) {
		UiObject::reset(__class__());
	}
	void reset_status() {
		m_origin_x = m_width / 2;
		m_origin_y = m_height / 2;
		m_grid_width = 32;
	}
	void get_board(RECT *pr) {
		pr->left   = (int)(m_origin_x - 7 * m_grid_width);
		pr->top    = (int)(m_origin_y - 7 * m_grid_width);
		pr->right  = (int)(m_origin_x + 7 * m_grid_width);;
		pr->bottom = (int)(m_origin_y + 7 * m_grid_width);;
	}
	void mouse_leave() override {
		m_on_drag = false;

		HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
		SetClassLongPtr(m_hwnd, GCLP_HCURSOR, (LONG_PTR)cursor);
	}
	void created() override {
		reset_status();
	}
	void double_click(int x, int y, int key_state) override {
		reset_status();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
	void mouse_press(int x, int y, int key_state) override {
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
	void mouse_release(int x, int y, int key_state) override {
		m_on_drag = false;

		HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
		SetClassLongPtr(m_hwnd, GCLP_HCURSOR, (LONG_PTR)cursor);
	}
	void mouse_move(int x, int y, int key_state) override {
		if (!m_on_drag) return;

		m_origin_x += x - m_drag_start_posx;
		m_origin_y += y - m_drag_start_posy;

		RECT board;
		get_board(&board);

		m_drag_start_posx = x;
		m_drag_start_posy = y;

		InvalidateRect(m_hwnd, NULL, FALSE);
	}
	void wheel_change(int x, int y, int delta) override {
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
	void render() override {
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

		POINT delta[] = {{0, 0}, {-4, -4}, {-4, 4}, {4, -4}, {4, 4}};
		int r = m_grid_width * 1.0 / 9;

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

		for (int i = 0; i < sizeof(delta) / sizeof(POINT); ++i) {
			POINT pt = {
				m_origin_x + delta[i].x * m_grid_width,
				m_origin_y + delta[i].y * m_grid_width
			};
			RECT rdot = { pt.x - r, pt.y - r, pt.x + r, pt.y + r };
			if (PtInRect(&rect, pt)) {
				FillRect(hmdc, &rdot, (HBRUSH)GetStockObject(BLACK_BRUSH));
			}
		}

		BitBlt(ps.hdc, 0, 0, m_width, m_height, hmdc, 0, 0, SRCCOPY);

		DeleteDC(hmdc);
		DeleteObject(hbitmap);

		EndPaint(m_hwnd, &ps);
	}
protected:
	bool m_on_drag;
	int m_drag_start_posx, m_drag_start_posy;
private:
	int m_origin_x, m_origin_y;
	int m_grid_width;
};
