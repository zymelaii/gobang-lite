#ifndef PLAYER_WOD_H
#define PLAYER_WOD_H

#include "../gobang/gobang.h"
#include "../ui/prototype.h"

class WeightOrientedDecision : public gobang::Player {
public:
	void prepare() override;
public:
	gobang::ui::UiObject::signal<void()> prepared;
};

#endif /*PLAYER_WOD_H*/
