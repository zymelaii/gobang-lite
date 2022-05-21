#ifndef UI_IMAGEVIEW_H
#define UI_IMAGEVIEW_H

#include "widget.h"

namespace gobang::ui {

class ImageView : public Widget {
	UI_OBJECT;
public:
	ImageView(int width, int height, const char *image_path);
	~ImageView();
public:
	void render() override;
private:
	size_t m_srcwidth, m_srcheight;
	uint8_t *m_bytes;
};

};

#endif /*UI_IMAGEVIEW_H*/
