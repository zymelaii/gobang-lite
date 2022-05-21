#ifndef UI_AEROPANE_H
#define UI_AEROPANE_H

#include "widget.h"

namespace gobang::ui {

class AeroPane : public Widget {
	UI_OBJECT;
public:
	AeroPane();
	AeroPane(int width, int height);
public:
	void blend(uint32_t fgcolor, double alpha);
	void set_border(bool draw_border = true);
public:
	virtual void render() override;
private:
	// {red, green, blue, gray}
	uint8_t m_fgvalue[4];
	double m_alpha;
	bool m_draw_border;
};

};

#endif /*UI_AEROPANE_H*/