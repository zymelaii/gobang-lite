#ifndef PLAYER_TRAMP_H
#define PLAYER_TRAMP_H

#include "../gobang/gobang.h"
#include "../ui/prototype.h"

class Tramp : public gobang::Player {
public:
	void prepare() override;
public:
	gobang::ui::UiObject::signal<void()> prepared;
};

#endif /*PLAYER_TRAMP_H*/
