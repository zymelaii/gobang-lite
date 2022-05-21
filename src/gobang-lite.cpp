#include "ui/widget.h"
#include "ui/button.h"
#include "ui/pane.h"
#include "view.h"
#include "gobang/player.h"

#include <stdio.h>

using namespace gobang::ui;

class GobangLiteApp : public Widget {
	UI_OBJECT;
public:
	GobangLiteApp(int argc, const char **argv)
		: Widget(700, 500) {
		UiObject::reset(__class__());
		setup();

		m_view = new View;

		m_btstart = new Button(80, 40);
		m_btstart->set_text("start");
		m_btstart->clicked.connect([this](int,int,int) {
			bool succeed = m_view->restart(false);
			if (!succeed) {
				MessageBox(m_hwnd,
					"    Chess game is being on.", " INFO ", MB_OK);
			}
		});

		auto m_pane = new Pane(100, 0);
		m_pane->move(10, 10);
		m_pane->setbkcolor(RGB(251, 228, 152));
		add_widget(m_pane);

		m_view->game()->finished.connect([this]() {
			MessageBox(m_hwnd,
				"    Chess game finished, restart" "\n"
				"the game if needed.", " INFO ", MB_OK);
			m_btstart->set_text("restart");
		});

		resized.connect([this, m_pane](int width, int height, int) {
			m_view->Widget::resize(width - m_pane->width() - 30, height - 20);
			m_view->move(width - m_view->width() - 10, 10);
		});

		resized.connect([this, m_pane](int, int height, int) {
			m_pane->resize(m_pane->width(), height - 20);
		});

		m_pane->resized.connect([this, m_pane](int width, int height, int) {
			m_btstart->move(
				(width - m_btstart->width()) / 2,
				(height - m_btstart->height()) / 2);
		});

		add_widget(m_view);
		add_widget(m_pane);
		m_pane->add_widget(m_btstart);

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
private:
	View *m_view;
	Button *m_btstart;
};

int main(int argc, const char *argv[]) {
	GobangLiteApp app(argc, argv);
	app.set_title("Gobang Lite");

	return app.exec();
}
