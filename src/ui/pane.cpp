#include "pane.h"

namespace gobang::ui {

Pane::Pane()
	: Widget() {
	UiObject::reset(__class__());
	setbkcolor(RGB(255, 255, 255));
}

Pane::Pane(int width, int height)
	: Widget(width, height) {
	UiObject::reset(__class__());
	setbkcolor(RGB(255, 255, 255));
}

void Pane::setbkcolor(uint32_t color) {
	m_bkcolor = color & 0x00ffffff;
	redraw();
}

void Pane::render() {
	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	HBRUSH brush = CreateSolidBrush(m_bkcolor);
	FillRect(ps.hdc, &ps.rcPaint, brush);
	DeleteObject(brush);

	EndPaint(m_hwnd, &ps);
}

};
