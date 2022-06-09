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
#include <stdio.h>
namespace fs = std::filesystem;

namespace gobang::ui {

ImageView::ImageView(int width, int height, const char *image_path)
	: Widget(width, height) {
	UiObject::reset(__class__());

	if (image_path != nullptr) {
		auto desc = load_as_bgra(image_path);
		if (desc.bytes == nullptr) {
			fprintf(stderr, "@%s stbi_load(...) failure: %s\n",
				get_class(), stbi_failure_reason());
			abort();
		}
		load(desc);
	} else {
		m_srcwidth  = 0;
		m_srcheight = 0;
		m_blend     = false;
		m_bytes     = nullptr;
	}

	created.connect([this](){
		if (m_blend || m_bytes == nullptr) {
			SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(NULL_BRUSH));
		}
	});

	resized.connect([this](int, int, int) {
		redraw();
	});
}

ImageView::~ImageView() {
	delete[] m_bytes;
}

ImageView::imdesc_t ImageView::load_as_bgra(const char *image_path) {
	int x, y, n;
	auto bytes = stbi_load(
		fs::absolute(image_path).string().c_str(),
		&x, &y, &n, 0);

	if (bytes == nullptr) return { };

	const size_t size = x * y;

	uint8_t *bgra = bytes;
	for (size_t i = 0; i < size; ++i, bgra += n) {
		uint8_t tmp = bgra[2];
		bgra[2] = bgra[0];
		bgra[0] = tmp;
	}

	bgra = new uint8_t[x * y * 4];
	if (n == 3) {
		uint8_t *p = bgra, *q = bytes;
		for (size_t i = 0; i < size; ++i, p += 4, q += 3) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = q[2];
			p[3] = 0xff;
		}
	} else if (n == 4) {
		memcpy(bgra, bytes, size * 4);
	}

	stbi_image_free(bytes);

	return { x, y, n, bgra };
}

ImageView::imdesc_t ImageView::load(imdesc_t desc) {
	if (desc.bytes == nullptr) return { };
	imdesc_t old_desc = { (int)m_srcwidth, (int)m_srcheight, m_blend ? 4 :3, m_bytes };
	m_srcwidth  = desc.width;
	m_srcheight = desc.height;
	m_blend     = desc.n == 4;
	m_bytes     = desc.bytes;

	redraw();

	return old_desc;
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

	HDC himdc = CreateCompatibleDC(ps.hdc);
	HBITMAP himage = CreateCompatibleBitmap(ps.hdc, m_srcwidth, m_srcheight);
	SelectObject(himdc, himage);

	HDC hmdc = CreateCompatibleDC(ps.hdc);
	HBITMAP hbmp = CreateCompatibleBitmap(ps.hdc, m_width, m_height);
	SelectObject(hmdc, hbmp);

	SetDIBits(himdc, himage, 0, m_srcheight, m_bytes, &bminfo, DIB_RGB_COLORS);

	SetStretchBltMode(hmdc, COLORONCOLOR);
	StretchBlt(
		hmdc, 0, 0, m_width, m_height,
		himdc, 0, 0, m_srcwidth, m_srcheight,
		SRCCOPY);

	if (m_blend) {
		BITMAPINFO bminfo = { };
		bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bminfo.bmiHeader.biPlanes = 1;
		bminfo.bmiHeader.biWidth = m_width;
		bminfo.bmiHeader.biHeight = -m_height;
		bminfo.bmiHeader.biBitCount = 32;
		bminfo.bmiHeader.biCompression = BI_RGB;

		auto bytes = new uint8_t[m_width * m_height * 4];
		GetDIBits(hmdc, hbmp, 0, m_height, bytes, &bminfo, DIB_RGB_COLORS);

		HWND hpwnd = GetParent(m_hwnd);
		HDC hpdc = GetDC(hpwnd);
		HDC hpmdc = CreateCompatibleDC(hpdc);
		HBITMAP hpbmp = CreateCompatibleBitmap(hpdc, m_width, m_height);
		SelectObject(hpmdc, hpbmp);

		RECT client;
		GetClientRect(m_hwnd, &client);
		BitBlt(hpmdc, 0, 0, m_width, m_height, hpdc, x(), y(), SRCCOPY);
		
		auto backs = new uint8_t[m_width * m_height * 4];
		GetDIBits(hpmdc, hpbmp, 0, m_height, backs, &bminfo, DIB_RGB_COLORS);

		ReleaseDC(GetParent(m_hwnd), hpdc);
		for (size_t i = 0; i < m_width * m_height; ++i) {
			bytes[i*4+0]=(bytes[i*4+0]*bytes[i*4+3]+backs[i*4+0]*(0xff-bytes[i*4+3]))>>8;
			bytes[i*4+1]=(bytes[i*4+1]*bytes[i*4+3]+backs[i*4+1]*(0xff-bytes[i*4+3]))>>8;
			bytes[i*4+2]=(bytes[i*4+2]*bytes[i*4+3]+backs[i*4+2]*(0xff-bytes[i*4+3]))>>8;
			bytes[i*4+3]=0xff;
		}
		SetDIBits(hmdc, hbmp, 0, m_height, bytes, &bminfo, DIB_RGB_COLORS);
		delete[] bytes;
		delete[] backs;

		DeleteDC(hpmdc);
		DeleteObject(hpbmp);
		ReleaseDC(hpwnd, hpdc);
	}

	paint(hmdc);

	BitBlt(ps.hdc, 0, 0, m_width, m_height, hmdc, 0, 0, SRCCOPY);

	DeleteDC(himdc);
	DeleteDC(hmdc);
	DeleteObject(himage);
	DeleteObject(hbmp);

	EndPaint(m_hwnd, &ps);
}

};
