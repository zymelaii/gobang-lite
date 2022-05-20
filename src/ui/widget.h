#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include "prototype.h"

#include <windows.h>
#include <windowsx.h>

namespace gobang::ui {

class Widget : public UiObject {
	UI_OBJECT;
public:
	Widget();
	Widget(int width, int height);
public:
	void resize(int width, int height);
	void move(int x, int y);
	void set_title(const char *title);

	void setup(const Widget *parent = nullptr);
public:
	void show(int type = SW_SHOW);
	bool add_widget(Widget *widget);
public:
	int width() const;
	int height() const;
public:
	virtual void created();
	virtual void moved(int x, int y);
	virtual void resized(int width, int height, int type);

	virtual void mouse_hover(int x, int y);
	virtual void mouse_leave();
	virtual void mouse_press(int x, int y, int key_state);
	virtual void mouse_release(int x, int y, int key_state);
	virtual void mouse_move(int x, int y, int key_state);
	virtual void click(int x, int y, int key_state);
	virtual void double_click(int x, int y, int key_state);
	virtual void wheel_change(int x, int y, int delta, int key_state);

	virtual void render();
protected:
	static LRESULT CALLBACK processor(HWND, UINT, WPARAM, LPARAM);
protected:
	int m_default_min_width, m_default_min_height;
	int m_default_max_width, m_default_max_height;
	int m_min_width, m_min_height;
	int m_max_width, m_max_height;
protected:
	HWND m_hwnd;
	size_t m_width, m_height;
	int m_posx, m_posy;
	char *m_title;
	size_t m_title_len;
private:
	bool m_setup;
};

};

#endif /*UI_WIDGET_H*/
