#ifndef UI_IMAGEVIEW_H
#define UI_IMAGEVIEW_H

#include "widget.h"

namespace gobang::ui {

class ImageView : public Widget {
	UI_OBJECT;
public:
	ImageView(int width, int height, const char *image_path);
	~ImageView();

	struct imdesc_t { int width, height, n; uint8_t *bytes; };
	static imdesc_t load_as_bgra(const char *image_path);

	imdesc_t load(imdesc_t desc);
public:
	void render() override;
public:
	UiObject::signal<void(HDC)> paint;
private:
	size_t m_srcwidth, m_srcheight;
	uint8_t *m_bytes;
	bool m_blend;
};

};

#endif /*UI_IMAGEVIEW_H*/
