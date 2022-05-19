#include "ui/widget.h"
#include "view.h"

using gobang::ui::Widget;
using gobang::ui::UiObject;

class GobangLiteApp : public Widget {
	UI_OBJECT;
public:
	GobangLiteApp(const char *title)
		: Widget(1000, 640) {
		UiObject::reset(__class__());
		set_title(title);
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
		int size = width() < height() ? width() : height();
		view->resize(size - 20, size - 20);
		view->move(width() - view->width() - 10, 10);
		m_view = view;
		add_widget(m_view);
	}
private:
	View *m_view;
};

int main(int argc, const char *argv[]) {
	GobangLiteApp app("Gobang Lite");
	app.set_view(new View);
	return app.exec();
}
