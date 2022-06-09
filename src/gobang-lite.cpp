#include "ui/widget.h"
#include "ui/button.h"
#include "ui/label.h"
#include "ui/pane.h"
#include "ui/aeropane.h"
#include "ui/imageview.h"
#include "view.h"
#include "termview.h"
#include "gobang/player.h"

using namespace gobang::ui;

class GobangLiteApp : public ImageView {
	UI_OBJECT;
public:
	GobangLiteApp(int argc, const char **argv)
		: ImageView(688, 552, "./assets/background.png") {
		UiObject::reset(__class__());
		setup();

		set_min_bound(588, 452);

		const size_t margin = 16;

		//! create children controls
		auto pane 			= new AeroPane(120, 0);
		auto placer 		= new AeroPane;
		auto term_view 		= new TermView;
		auto billboard		= new Label;
		auto btstart 		= new Button(80, 40);
		auto view 			= new View;
		auto view_decorate 	= new Pane;

		//! initial config
		pane->move(margin, margin);
		pane->blend(RGB(255, 0, 0), 0.1);
		placer->blend(RGB(0, 0, 255), 0.1);
		billboard->setbkmode(true);
		btstart->set_text("start");
		view_decorate->setbkmode(true);

		//! connect slots
		using namespace std::placeholders;
		resized.connect([this, margin, pane, placer](int width, int height, int) {
			pane->resize(pane->width(), height - margin * 2);
			placer->resize(width - pane->width() - margin * 3, height - margin * 2);
			placer->move(width - placer->width() - margin, margin);
		});
		key_down.connect(std::bind(&view->Widget::key_down.operator(), &view->Widget::key_down, _1));
		pane->resized.connect([this, term_view, btstart, billboard](int width, int height, int) {
			term_view->resize(width - 20, width - 20);
			term_view->move((width - term_view->width()) / 2, 10);
			btstart->move((width - btstart->width()) / 2, (height - btstart->height()) / 2);
			billboard->resize(width - 20, width - 20);
			billboard->move((width - term_view->width()) / 2, (
				btstart->y() + term_view->y() + term_view->height() - billboard->height()) / 2);
		});
		placer->resized.connect([this, margin, placer, view_decorate](int width, int height, int) {
			view_decorate->resize(width - margin, height - margin);
			view_decorate->move(margin / 2, margin / 2);
		});
		term_view->created.connect([term_view]() {
			term_view->push("./assets/black-chess.png");
			term_view->push("./assets/white-chess.png");
		});
		btstart->clicked.connect([this, billboard, view, term_view](int, int, int) {
			bool succeed = view->restart(false);
			if (!succeed) {
				billboard->set_text("当前棋局正在进行中！");
				return;
			}
			billboard->set_text("");
			view->redraw();
			term_view->switch_to(0);
		});
		view->game()->finished.connect([this, view, btstart, term_view, billboard]() {
			billboard->set_text(view->game()->get_winner() == ChessType::black
				? "黑方胜利！" : "白方胜利！");
			btstart->set_text("restart");
		});
		view->game()->dropped.connect([this, view, term_view](int, int, ChessType type) {
			if (view->game()->get_winner() == ChessType::nil) {
				term_view->switch_to(type == ChessType::black ? 1 : 0);
			}
		});
		view->game()->caught_forbidden_hand.connect([billboard](int row, int col) {
			printf("caught forbidden hand at (%d,%d)\n", row, col);
			billboard->set_text("禁手警告！");
		});
		view_decorate->resized.connect([this, margin, view](int width, int height, int) {
			int margin = 10;
			view->resize(width - margin * 2, height - margin * 2);
			view->move(margin, margin);
		});
		view_decorate->paint.connect(std::bind(&GobangLiteApp::paint_decoration, view_decorate, _1));

		add_widget(pane);
		add_widget(placer);
		pane->add_widget(term_view);
		pane->add_widget(billboard);
		pane->add_widget(btstart);
		placer->add_widget(view_decorate);
		view_decorate->add_widget(view);

		resized(m_width, m_height, 0);
	}
	int exec() {
		show();
		MSG msg = { };
		while (GetMessage(&msg, NULL, 0, 0) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return msg.wParam;
	}
	static void paint_decoration(Widget *widget, HDC hdc) {
		HPEN oldpen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
		int pen_width[2] = { 3, 1 };

		int len[2] = { 15, 7 };
		int margin[2] = { 0, 5 };

		for (int i = 0; i < 2; ++i) {
			HPEN pen = CreatePen(PS_SOLID, pen_width[i], 0);
			SelectObject(hdc, pen);

			RECT rect = {
				margin[i], margin[i],
				widget->width() - margin[i] - 1,
				widget->height() - margin[i] - 1
			};

			MoveToEx(hdc, rect.left, rect.top + len[i], NULL);
			LineTo(hdc, rect.left, rect.top);
			LineTo(hdc, rect.left + len[i], rect.top);

			MoveToEx(hdc, rect.right - len[i], rect.top, NULL);
			LineTo(hdc, rect.right, rect.top);
			LineTo(hdc, rect.right, rect.top + len[i]);

			MoveToEx(hdc, rect.right, rect.bottom - len[i], NULL);
			LineTo(hdc, rect.right, rect.bottom);
			LineTo(hdc, rect.right - len[i], rect.bottom);

			MoveToEx(hdc, rect.left + len[i], rect.bottom, NULL);
			LineTo(hdc, rect.left, rect.bottom);
			LineTo(hdc, rect.left, rect.bottom - len[i]);

			DeleteObject(pen);
		}

		SelectObject(hdc, oldpen);
	}
};

int main(int argc, const char *argv[]) {
	GobangLiteApp app(argc, argv);
	app.set_title("Gobang Lite");
	return app.exec();
}
