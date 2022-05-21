#include "ui/widget.h"
#include "ui/button.h"
#include "ui/pane.h"
#include "ui/aeropane.h"
#include "ui/imageview.h"
#include "view.h"
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

		auto view = new View;

		auto btstart = new Button(80, 40);
		btstart->set_text("start");
		btstart->clicked.connect([this, view](int, int, int) {
			bool succeed = view->restart(false);
			if (!succeed) {
				MessageBox(m_hwnd,
					"    Chess game is being on.", " INFO ", MB_OK);
			}
		});

		auto view_decorate = new Pane;
		view_decorate->setbkmode(true);

		auto placer = new AeroPane;
		placer->blend(RGB(0, 0, 255), 0.1);

		auto pane = new AeroPane(120, 0);
		pane->move(margin, margin);
		pane->blend(RGB(255, 0, 0), 0.1);

		resized.connect([this, margin, pane, placer](int width, int height, int) {
			placer->resize(width - pane->width() - margin * 3, height - margin * 2);
			placer->move(width - placer->width() - margin, margin);
		});

		resized.connect([this, margin, pane](int, int height, int) {
			pane->resize(pane->width(), height - margin * 2);
		});

		view_decorate->resized.connect([this, margin, view](int width, int height, int) {
			int margin = 10;
			view->resize(width - margin * 2, height - margin * 2);
			view->move(margin, margin);
		});

		view_decorate->paint.connect([this, view_decorate](HDC hdc) {
			HPEN oldpen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
			int pen_width[2] = { 3, 1 };

			int len[2] = { 15, 7 };
			int margin[2] = { 0, 5 };

			for (int i = 0; i < 2; ++i) {
				HPEN pen = CreatePen(PS_SOLID, pen_width[i], 0);
				SelectObject(hdc, pen);

				RECT rect = {
					margin[i], margin[i],
					view_decorate->width() - margin[i] - 1,
					view_decorate->height() - margin[i] - 1
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
		});

		placer->resized.connect([this, margin, placer, view_decorate](int width, int height, int) {
			view_decorate->resize(width - margin, height - margin);
			view_decorate->move(margin / 2, margin / 2);
		});

		pane->resized.connect([this, btstart, pane](int width, int height, int) {
			btstart->move((width - btstart->width()) / 2, (height - btstart->height()) / 2);
		});

		view->game()->finished.connect([this, btstart]() {
			MessageBox(m_hwnd,
				"    Chess game finished, restart" "\n"
				"the game if needed.", " INFO ", MB_OK);
			btstart->set_text("restart");
		});

		add_widget(pane);
		add_widget(placer);
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
};

int main(int argc, const char *argv[]) {
	GobangLiteApp app(argc, argv);
	app.set_title("Gobang Lite");

	return app.exec();
}
