#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "widget.h"

#include <stdint.h>
#include <stddef.h>

namespace gobang::ui {

class Button : public Widget {
	UI_OBJECT;
public:
	Button();
	Button(int width, int height);
public:
	void set_text(const char *text);
	void setbgcolor(uint32_t color);
	void setfgcolor(uint32_t color);
public:
	void mouse_hover(int x, int y) override;
	void mouse_leave() override;
	void mouse_press(int x, int y, int key_state) override;
	void mouse_release(int x, int y, int key_state) override;
	virtual void render() override;
private:
	char m_text[64];
	uint32_t m_bgcolor_c;
	uint32_t m_fgcolor_c;
	uint32_t m_bgcolor;
	uint32_t m_fgcolor;
	uint32_t m_hover_color;
	bool m_after_hover;
};

};

#endif /*UI_BUTTON_H*/
