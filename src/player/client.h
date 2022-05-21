#ifndef PLAYER_CLIENT_H
#define PLAYER_CLIENT_H

#include "../gobang/gobang.h"
#include "../ui/prototype.h"

class ClientUser : public gobang::Player {
public:
	void prepare() override;
public:
	gobang::ui::UiObject::signal<void()> prepared;
};

#endif /*PLAYER_CLIENT_H*/
