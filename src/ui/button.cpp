#include "button.h"

#include <string.h>
#include <stdio.h>

namespace gobang::ui {

Button::Button()
	: Widget() {
	UiObject::reset(__class__());

	created.connect([this]() {
		SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)NULL);
	});

	enable_track();

	m_text[0] = '\0';

	setfgcolor(RGB(39, 125, 191));
	setbgcolor(RGB(255, 255, 255));

	m_bgcolor_c = m_bgcolor;
	m_fgcolor_c = m_fgcolor;
}

Button::Button(int width, int height)
	: Widget(width, height) {
	UiObject::reset(__class__());

	created.connect([this]() {
		SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)NULL);
	});

	enable_track();

	m_text[0] = '\0';

	setfgcolor(RGB(39, 125, 191));
	setbgcolor(RGB(255, 255, 255));

	m_bgcolor_c = m_bgcolor;
	m_fgcolor_c = m_fgcolor;
}

void Button::set_text(const char *text) {
	snprintf(m_text, sizeof(m_text), text);

	redraw();
}

void Button::setbgcolor(uint32_t color) {
	m_bgcolor = color & 0x00ffffff;

	int br = GetRValue(m_bgcolor);
	int bg = GetGValue(m_bgcolor);
	int bb = GetBValue(m_bgcolor);
	int fr = GetRValue(m_fgcolor);
	int fg = GetGValue(m_fgcolor);
	int fb = GetBValue(m_fgcolor);

	m_hover_color = RGB(
		0.7 * br + 0.3 * fr,
		0.7 * bg + 0.3 * fg,
		0.7 * bb + 0.3 * fb);

	redraw();
}

void Button::setfgcolor(uint32_t color) {
	m_fgcolor = color & 0x00ffffff;

	int br = GetRValue(m_bgcolor);
	int bg = GetGValue(m_bgcolor);
	int bb = GetBValue(m_bgcolor);
	int fr = GetRValue(m_fgcolor);
	int fg = GetGValue(m_fgcolor);
	int fb = GetBValue(m_fgcolor);

	m_hover_color = RGB(
		0.7 * br + 0.3 * fr,
		0.7 * bg + 0.3 * fg,
		0.7 * bb + 0.3 * fb);

	redraw();
}

void Button::mouse_hover(int x, int y) {
	m_fgcolor_c = m_bgcolor;
	m_bgcolor_c = m_hover_color;
	m_after_hover = true;

	redraw();
}

void Button::mouse_leave() {
	m_fgcolor_c = m_fgcolor;
	m_bgcolor_c = m_bgcolor;
	m_after_hover = false;

	redraw();
}

void Button::mouse_press(int x, int y, int key_state) {
	m_fgcolor_c = m_bgcolor;
	m_bgcolor_c = m_fgcolor;

	redraw();
}

void Button::mouse_release(int x, int y, int key_state) {
	if (m_after_hover) {
		m_fgcolor_c = m_bgcolor;
		m_bgcolor_c = m_hover_color;
	} else {
		m_fgcolor_c = m_fgcolor;
		m_bgcolor_c = m_bgcolor;
	}

	redraw();
}

void Button::render() {
	POINT origin;
	RECT rc_button;
	{
		RECT rc_window;
		GetWindowRect(m_hwnd, &rc_window);
		GetClientRect(m_hwnd, &rc_button);
		origin.x = rc_window.left;
		origin.y = rc_window.top;
		ScreenToClient(GetParent(m_hwnd), &origin);
	}

	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	HDC hmdc = CreateCompatibleDC(ps.hdc);
	HBITMAP hbitmap = CreateCompatibleBitmap(ps.hdc, m_width, m_height);
	HBRUSH hbrush = CreateSolidBrush(m_bgcolor_c);
	HPEN hpen = CreatePen(PS_SOLID, 1, m_fgcolor_c);
	SelectObject(hmdc, hbitmap);
	SelectObject(hmdc, hbrush);
	SelectObject(hmdc, hpen);
	SetTextColor(hmdc, m_fgcolor_c);
	SetBkMode(hmdc, TRANSPARENT);

	BitBlt(hmdc, 0, 0, m_width, m_height, ps.hdc, 0, 0, SRCCOPY);
	RoundRect(hmdc, 0, 0, m_width, m_height, 10, 10);
	DrawText(hmdc, m_text, strlen(m_text), &rc_button, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	BitBlt(ps.hdc, 0, 0, m_width, m_height, hmdc, 0, 0, SRCCOPY);

	DeleteObject(hpen);
	DeleteObject(hbrush);
	DeleteObject(hbitmap);
	DeleteDC(hmdc);

	EndPaint(m_hwnd, &ps);
}

};
