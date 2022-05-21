#include "aeropane.h"

#include <stdlib.h>

namespace gobang::ui {

AeroPane::AeroPane()
	: Widget() {
	UiObject::reset(__class__());

	created.connect([this]() {
		SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)NULL);
		srand((intptr_t)this);
	});

	blend(0, 0.05);
	set_border();
}

AeroPane::AeroPane(int width, int height)
	: Widget(width, height) {
	UiObject::reset(__class__());

	created.connect([this]() {
		SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)NULL);
		srand((intptr_t)this);
	});

	blend(0, 0.05);
	set_border();
}

void AeroPane::blend(uint32_t fgcolor, double alpha) {
	if (alpha < 0.0) alpha = 0.0;
	if (alpha > 1.0) alpha = 1.0;
	m_fgvalue[0] = GetRValue(fgcolor);
	m_fgvalue[1] = GetGValue(fgcolor);
	m_fgvalue[2] = GetBValue(fgcolor);
	m_fgvalue[3] = (m_fgvalue[0] * 77 + m_fgvalue[1] * 151 + m_fgvalue[2] * 28) >> 8;
	m_alpha = alpha;

	redraw();
}

void AeroPane::set_border(bool draw_border) {
	if (m_draw_border == draw_border) return;
	m_draw_border = draw_border;
	redraw();
}

void AeroPane::render() {
	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	HDC hdc = ps.hdc;

	HDC hmdc = CreateCompatibleDC(hdc);
	HBITMAP hbmp = CreateCompatibleBitmap(hdc, m_width, m_height);
	SelectObject(hmdc, hbmp);
	SelectObject(hmdc, (HBRUSH)GetStockObject(NULL_BRUSH));

	BitBlt(hmdc, 0, 0, m_width, m_height, hdc, 0, 0, SRCCOPY);

	HRGN clipped = CreateRoundRectRgn(0, 0, m_width, m_height, 16, 16);
	SelectObject(hmdc, clipped);

	BITMAPINFO bminfo = { };
	bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits(hmdc, hbmp, 0, m_height, NULL, &bminfo, DIB_RGB_COLORS);
	auto bytes = new uint8_t[m_width * m_height * 4];
	GetDIBits(hmdc, hbmp, 0, m_height, bytes, &bminfo, DIB_RGB_COLORS);

	const int r = 4;

	auto get = [&](int row, int col, int i) {
		if (row < 0) row = 0;
		if (row >= m_height) row = m_height - 1;
		if (col < 0) col = 0;
		if (col >= m_width) col = m_width - 1;
		return bytes[(row * m_width + col) * 4 + i];
	};

	auto set = [&](int row, int col, int i, uint8_t value) {
		if (row < 0) return;
		if (row >= m_height) return;
		if (col < 0) return;
		if (col >= m_width) return;
		bytes[(row * m_width + col) * 4 + i] = value;
	};

	for (int row = 0; row < m_height; ++row) {
		for (int col = 0; col < m_width; ++col) {
			int dx = rand() % (2 * r + 1) - r;
			int dy = rand() % (2 * r + 1) - r;

			int b = get(row + dy, col + dx, 0);
			int g = get(row + dy, col + dx, 1);
			int r = get(row + dy, col + dx, 2);

			set(row, col, 0, b * (1 - m_alpha) + m_fgvalue[2] * m_alpha);
			set(row, col, 1, g * (1 - m_alpha) + m_fgvalue[1] * m_alpha);
			set(row, col, 2, r * (1 - m_alpha) + m_fgvalue[0] * m_alpha);
		}
	}

	if (m_draw_border) {
		SetDIBits(hmdc, hbmp, 0, m_height, bytes, &bminfo, DIB_RGB_COLORS);
		RoundRect(hmdc, 1, 1, m_width - 1, m_height - 1, 16, 16);
		GetDIBits(hmdc, hbmp, 0, m_height, bytes, &bminfo, DIB_RGB_COLORS);
	}

	double kernel2[3][3] = {
		{ 0.094742, 0.118318, 0.094742 },
		{ 0.118318, 0.147761, 0.118318 },
		{ 0.094742, 0.118318, 0.094742 },
	};

	for (int row = 0; row < m_height; ++row) {
		for (int col = 0; col < m_width; ++col) {
			double value[3] = { 0.0, 0.0, 0.0 };
			for (int dy = -1; dy <= 1; ++dy) {
				for (int dx = -1; dx <= 1; ++dx) {
					value[0] += get(row + dy, col + dx, 0) * kernel2[dy + 1][dx + 1];
					value[1] += get(row + dy, col + dx, 1) * kernel2[dy + 1][dx + 1];
					value[2] += get(row + dy, col + dx, 2) * kernel2[dy + 1][dx + 1];
				}
			}
			set(row, col, 0, value[0]);
			set(row, col, 1, value[1]);
			set(row, col, 2, value[2]);
		}
	}

	SetDIBits(hmdc, hbmp, 0, m_height, bytes, &bminfo, DIB_RGB_COLORS);
	BitBlt(hdc, 0, 0, m_width, m_height, hmdc, 0, 0, SRCCOPY);
	delete[] bytes;

	DeleteObject(clipped);
	DeleteObject(hbmp);
	DeleteDC(hmdc);

	EndPaint(m_hwnd, &ps);
}

};
