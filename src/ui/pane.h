#ifndef UI_PANE_H
#define UI_PANE_H

#include "widget.h"

#include <stdint.h>
#include <stddef.h>

namespace gobang::ui {

class Pane : public Widget {
	UI_OBJECT;
public:
	Pane();
	Pane(int width, int height);
public:
	void setbkcolor(uint32_t color);
public:
	virtual void render() override;
private:
	uint32_t m_bkcolor;
};

};

#endif /*UI_PANE_H*/