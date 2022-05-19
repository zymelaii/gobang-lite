#include "ui/widget.h"
#include "gobang/board.h"
#include <tuple>

using gobang::ui::Widget;
using gobang::ui::UiObject;
using gobang::BoardStatus;

class View : public Widget {
	UI_OBJECT;
public:
	View() {
		UiObject::reset(__class__());
	}
	View(int width, int height)
		: Widget(width, height) {
		UiObject::reset(__class__());
	}

	void reset_status();
	void set_origin(int row, int col);
	std::tuple<int, int> get_nearest(int x, int y);
	void get_board(RECT *rc);
	void get_drop(int row, int col, POINT *pt);

	void begin_drag(int x, int y);
	void on_drag(int x, int y);
	void end_drag();
public:
	void created() override;
	void resized(int width, int height, int type) override;

	void double_click(int x, int y, int key_state) override;

	void mouse_press(int x, int y, int key_state) override;
	void mouse_release(int x, int y, int key_state) override;
	void mouse_leave() override;

	void mouse_move(int x, int y, int key_state) override;

	void wheel_change(int x, int y, int delta) override;

	void render() override;
protected:
	bool m_on_drag;
	int m_drag_start_posx, m_drag_start_posy;

	int m_predrop_row, m_predrop_col;
private:
	int m_origin_x, m_origin_y;
	int m_origin_row, m_origin_col;
	int m_grid_width;
private:
	BoardStatus m_bs;
};
