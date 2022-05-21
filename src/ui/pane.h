#ifndef UI_PANE_H
#define UI_PANE_H

#include "widget.h"

namespace gobang::ui {

class Pane : public Widget {
	UI_OBJECT;
public:
	Pane();
	Pane(int width, int height);
public:
	void setbkcolor(uint32_t color);
	void setbkmode(bool transparent);
public:
	virtual void render() override;
public:
	UiObject::signal<void(HDC)> paint;
private:
	uint32_t m_bkcolor;
	uint32_t m_transparent;
};

};

#endif /*UI_PANE_H*/