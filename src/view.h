#include "ui/widget.h"
#include "ui/imageview.h"
#include "gobang/gobang.h"
#include "player/client.h"
#include "player/tramp.h"
#include "player/ai_v1.h"

#include <tuple>

using gobang::ui::ImageView;
using gobang::ui::UiObject;
using gobang::BoardStatus;
using gobang::ChessType;

constexpr inline const char *background = "./assets/wooden-material.png";

class View : public ImageView {
	UI_OBJECT;
public:
	View()
		: ImageView(0, 0, background) {
		init();
	}
	View(int width, int height)
		: ImageView(width, height, background) {
		init();
	}

	void init() {
		using namespace std::placeholders;
		UiObject::reset(__class__());

		enable_track();

		join_players();

		created.connect(std::bind(&View::reset_status, this));
		created.connect([this]() {
			m_preview_cursor = new ImageView(0, 0, "./assets/cursor.png");
			m_preview_cursor->resize(m_grid_width * 0.8, m_grid_width * 0.8);
			add_widget(m_preview_cursor);
			m_preview_cursor->show(SW_HIDE);
			m_preview_cursor->clicked.connect([this](int x, int y, int key_state) {
				clicked(x, y, key_state);
			});
		});
		resized.connect(std::bind(&View::reset_status, this));
		paint.connect(std::bind(&View::draw_board, this, _1));
		double_clicked.connect(std::bind(&View::reset_status, this));

		clicked.connect([this](int, int, int) {
			if (!m_is_user_term) return;
			if (m_predrop_row != -1 && m_predrop_col != -1) {
				m_user->drop(m_predrop_row, m_predrop_col);
				m_predrop_row = m_predrop_col = -1;
			}
		});

		Widget::key_down.connect(std::bind(&View::key_down, this, _1));
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

	void key_down(int key);

	void wheel_change(int x, int y, int delta, int key_state) override;

	void draw_board(HDC hdc);
protected:
	bool m_on_drag;
	int m_drag_start_posx, m_drag_start_posy;

	int m_predrop_row, m_predrop_col;
	bool m_drop_on_keyctrl;
	ImageView *m_preview_cursor;

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
