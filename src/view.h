#include "ui/widget.h"
#include "gobang/gobang.h"
#include "player/client.h"
#include "player/tramp.h"

#include <tuple>

using gobang::ui::Widget;
using gobang::ui::UiObject;
using gobang::BoardStatus;
using gobang::ChessType;

class View : public Widget {
	UI_OBJECT;
public:
	View()
		: Widget() {
		init();
	}
	View(int width, int height)
		: Widget(width, height) {
		init();
	}

	void init() {
		using namespace std::placeholders;
		UiObject::reset(__class__());

		enable_track();

		join_players();

		created.connect(std::bind(&View::reset_status, this));
		resized.connect(std::bind(&View::reset_status, this));
		double_clicked.connect(std::bind(&View::reset_status, this));

		clicked.connect([this](int, int, int) {
			if (!m_is_user_term) return;
			if (m_predrop_row != -1 && m_predrop_col != -1) {
				m_user->drop(m_predrop_row, m_predrop_col);
				m_predrop_row = m_predrop_col = -1;
			}
		});
	}

	void reset_status();
	void set_origin(int row, int col);
	std::tuple<int, int> get_nearest(int x, int y);
	gobang::Game* game();
	void get_board(RECT *rc);
	void get_drop(int row, int col, POINT *pt);

	bool restart(bool force = false);
protected:
	void join_players();

	void begin_drag(int x, int y);
	void on_drag(int x, int y);
	void end_drag();
public:
	void mouse_press(int x, int y, int key_state) override;
	void mouse_release(int x, int y, int key_state) override;
	void mouse_leave() override;

	void mouse_move(int x, int y, int key_state) override;

	void wheel_change(int x, int y, int delta, int key_state) override;

	void render() override;
protected:
	bool m_on_drag;
	int m_drag_start_posx, m_drag_start_posy;

	int m_predrop_row, m_predrop_col;

	ChessType m_user_term;
	bool m_is_user_term;
private:
	int m_origin_x, m_origin_y;
	int m_origin_row, m_origin_col;
	int m_grid_width;
private:
	gobang::Game m_game;

	ClientUser *m_user;
	gobang::Player *m_white_player;
	gobang::Player *m_black_player;
};
