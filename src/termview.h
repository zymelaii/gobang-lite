#include "ui/imageview.h"

using namespace gobang::ui;

class TermView : public ImageView {
	UI_OBJECT;
public:
	TermView() :
		ImageView(0, 0, nullptr),
		m_current_image_id(-1) {
		UiObject::reset(__class__());

		paint.connect([this](HDC hdc) {
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, this->width() - 1, 0);
			LineTo(hdc, this->width() - 1, this->height() - 1);
			LineTo(hdc, 0, this->height() - 1);
			LineTo(hdc, 0, 0);
		});
	}
	~TermView() {
		for (int i = 0; i < m_image_set.size(); ++i) {
			if (i != m_current_image_id) {
				delete[] m_image_set[i].bytes;
			}
		}
	}
public:
	int push(const char *image_path) {
		auto desc = ImageView::load_as_bgra(image_path);
		if (desc.bytes == nullptr) {
			return -1;
		}
		m_image_set.push_back(desc);
		return m_image_set.size() - 1;
	}
	bool switch_to(int image_id) {
		if (image_id == -1) {
			m_current_image_id = image_id;
			redraw();
			return true;
		}
		if (image_id >= 0 && image_id <= m_image_set.size()
			&& image_id != m_current_image_id) {
			load(m_image_set[image_id]);
			m_current_image_id = image_id;
			redraw();
			return true;
		}
		return false;
	}
public:
	void render() override {
		if (m_current_image_id == -1) {
			PAINTSTRUCT ps;
			BeginPaint(m_hwnd, &ps);
			paint(ps.hdc);
			EndPaint(m_hwnd, &ps);
			return;
		}
		ImageView::render();
	}
private:
	int m_current_image_id;
	std::vector<ImageView::imdesc_t> m_image_set;
};
