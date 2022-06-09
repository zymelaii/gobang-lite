#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include "prototype.h"

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <tuple>

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
	void enable_track(bool enable = true);
	virtual void redraw(bool erase = false);

	void set_min_bound(int width = -1, int height = -1);
	void set_max_bound(int width = -1, int height = -1);

	void setup(const Widget *parent = nullptr);
public:
	void show(int type = SW_SHOW);
	bool add_widget(Widget *widget);
public:
	int width() const;
	int height() const;
	int x() const;
	int y() const;

	std::tuple<int, int> get_min_bound() const;
	std::tuple<int, int> get_max_bound() const;

	Widget* parent();
public: // signals
	UiObject::signal<void()> created;
	UiObject::signal<void(int,int)> moved;
	UiObject::signal<void(int,int,int)> resized;
	UiObject::signal<void(int,int,int)> clicked;
	UiObject::signal<void(int,int,int)> double_clicked;
	UiObject::signal<void(int)> key_down;
public: // events
	virtual void mouse_hover(int x, int y);
	virtual void mouse_leave();
	virtual void mouse_press(int x, int y, int key_state);
	virtual void mouse_release(int x, int y, int key_state);
	virtual void mouse_move(int x, int y, int key_state);
	virtual void wheel_change(int x, int y, int delta, int key_state);

	virtual void render();
protected:
	static LRESULT CALLBACK dispatch(HWND, UINT, WPARAM, LPARAM);
protected:
	int m_min_width, m_min_height;
	int m_max_width, m_max_height;

	bool m_preclicked;
	int m_clicked_posx, m_clicked_posy;

	bool m_enable_track;
	bool m_on_tracking;
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
