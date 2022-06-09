#ifndef UI_LABEL_H
#define UI_LABEL_H

#include "widget.h"

namespace gobang::ui {

class Label : public Widget {
	UI_OBJECT;
public:
	Label();
	Label(int width, int height);
public:
	void setfgcolor(uint32_t color);
	void setbkcolor(uint32_t color);
	void setbkmode(bool transparent);
	void set_text(const char *text);
public:
	virtual void render() override;
private:
	uint32_t m_fgcolor;
	uint32_t m_bkcolor;
	uint32_t m_transparent;
	char *m_text;
};

};

#endif /*UI_LABEL_H*/