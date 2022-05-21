#include "imageview.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "../3rdparty/stb_image.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace gobang::ui {

ImageView::ImageView(int width, int height, const char *image_path)
	: Widget(width, height) {
	UiObject::reset(__class__());

	int x, y, n;
	m_bytes = stbi_load(
		fs::absolute(image_path).string().c_str(),
		&x, &y, &n, 4);

	if (m_bytes == nullptr) {
		fprintf(stderr, "@%s stbi_load(...) failure: %s\n",
			get_class(), stbi_failure_reason());
		abort();
	}

	m_srcwidth  = x;
	m_srcheight = y;

	const size_t size = m_srcwidth * m_srcheight;
	uint8_t *argb = m_bytes;
	for (size_t i = 0; i < size; ++i, argb += 4) {
		argb[3] = argb[0];
		argb[0] = argb[2];
		argb[2] = argb[3];
		argb[3] = 0xff;
	}

	resized.connect([this](int, int, int) {
		redraw();
	});
}

ImageView::~ImageView() {
	stbi_image_free(m_bytes);
}

void ImageView::render() {
	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	BITMAPINFO bminfo = { };
	bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biWidth = m_srcwidth;
	bminfo.bmiHeader.biHeight = -m_srcheight;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;

	HDC hmdc = CreateCompatibleDC(ps.hdc);
	HBITMAP hbmp = CreateCompatibleBitmap(ps.hdc, m_srcwidth, m_srcheight);
	SelectObject(hmdc, hbmp);

	SetDIBits(hmdc, hbmp, 0, m_srcheight, m_bytes, &bminfo, DIB_RGB_COLORS);

	SetStretchBltMode(ps.hdc, COLORONCOLOR);
	StretchBlt(
		ps.hdc, 0, 0, m_width, m_height,
		hmdc, 0, 0, m_srcwidth, m_srcheight,
		SRCCOPY);

	EndPaint(m_hwnd, &ps);
}

};
