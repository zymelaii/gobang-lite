#include "ui/widget.h"
#include "view.h"
#include <stdio.h>

using gobang::ui::Widget;
using gobang::ui::UiObject;

class GobangLiteApp : public Widget {
	UI_OBJECT;
public:
	GobangLiteApp(int argc, const char **argv)
		: Widget(700, 500) {
		UiObject::reset(__class__());
		setup();
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
	void set_view(View *view) {
		m_view = view;
		resized(m_width, m_height, 0);
		add_widget(m_view);
	}
public:
	void resized(int width, int height, int type) override {
		int size = width < height ? width : height;
		m_view->resize(size - 20, size - 20);
		m_view->move(width - m_view->width() - 10, 10);
	}
private:
	View *m_view;
};

#include <stdio.h>
int main(int argc, const char *argv[]) {
	GobangLiteApp app(argc, argv);
	app.set_title("Gobang Lite *家徒四壁限定版");

	app.set_view(new View);

	return app.exec();
}
