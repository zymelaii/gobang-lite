#include "label.h"

#include <string.h>
#include <malloc.h>

namespace gobang::ui {

Label::Label()
	: Widget(), m_text(nullptr), m_fgcolor(0) {
	UiObject::reset(__class__());

	created.connect([this]() {
		SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)NULL);
	});

	m_transparent = false;

	setbkcolor(RGB(255, 255, 255));
}

Label::Label(int width, int height)
	: Widget(width, height), m_text(nullptr), m_fgcolor(0) {
	UiObject::reset(__class__());

	created.connect([this]() {
		SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)NULL);
	});

	m_transparent = false;

	setbkcolor(RGB(255, 255, 255));
}

void Label::setfgcolor(uint32_t color) {
	if (m_fgcolor != (color & 0x00ffffff)) {
		m_fgcolor = color & 0x00ffffff;
		redraw();
	}
}

void Label::setbkcolor(uint32_t color) {
	m_bkcolor = color & 0x00ffffff;
	if (!m_transparent) {
		redraw();
	}
}

void Label::setbkmode(bool transparent) {
	if (transparent == m_transparent) return;
	m_transparent = transparent;

	show(SW_HIDE);
	show();
}

void Label::set_text(const char *text) {
	if (m_text != NULL) {
		if (strcmp(m_text, text) == 0) return;
		free(m_text);
	}
	m_text = strdup(text);
	if (m_transparent) {
		parent()->redraw();
	}
	redraw();
}

void Label::render() {
	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	if (!m_transparent) {
		HBRUSH brush = CreateSolidBrush(m_bkcolor);
		FillRect(ps.hdc, &ps.rcPaint, brush);
		DeleteObject(brush);
	}

	if (m_text != NULL) {
		RECT rect, bound;
		GetClientRect(m_hwnd, &rect);
		GetClientRect(m_hwnd, &bound);
		HFONT hfont = CreateFont(
			16, 0, 0, 0, FW_DONTCARE, false, false, false,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
			VARIABLE_PITCH, "Courier New");
		SelectObject(ps.hdc, hfont);
		SetTextColor(ps.hdc, m_fgcolor);
		SetBkMode(ps.hdc, TRANSPARENT);
		int text_height = DrawText(ps.hdc, m_text, strlen(m_text), &bound,
			DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER);
		rect.top += (height() - bound.bottom) / 2;
		DrawText(ps.hdc, m_text, strlen(m_text), &rect,
			DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER);
		DeleteObject(hfont);
	}

	EndPaint(m_hwnd, &ps);
}

};
