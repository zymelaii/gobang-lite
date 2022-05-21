#include "pane.h"

namespace gobang::ui {

Pane::Pane()
	: Widget() {
	UiObject::reset(__class__());

	created.connect([this]() {
		SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)NULL);
	});

	m_transparent = false;
	setbkcolor(RGB(255, 255, 255));
}

Pane::Pane(int width, int height)
	: Widget(width, height) {
	UiObject::reset(__class__());

	created.connect([this]() {
		SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)NULL);
	});

	m_transparent = false;
	setbkcolor(RGB(255, 255, 255));
}

void Pane::setbkcolor(uint32_t color) {
	m_bkcolor = color & 0x00ffffff;
	if (!m_transparent) {
		redraw();
	}
}

void Pane::setbkmode(bool transparent) {
	if (transparent == m_transparent) return;
	m_transparent = transparent;

	show(SW_HIDE);
	show();
}

void Pane::render() {
	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	if (!m_transparent) {
		HBRUSH brush = CreateSolidBrush(m_bkcolor);
		FillRect(ps.hdc, &ps.rcPaint, brush);
		DeleteObject(brush);
	}

	paint(ps.hdc);

	EndPaint(m_hwnd, &ps);
}

};
