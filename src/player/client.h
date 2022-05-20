#ifndef PLAYER_CLIENT_H
#define PLAYER_CLIENT_H

#include "../gobang/gobang.h"

#include <functional>

class ClientUser : public gobang::Player {
public:
	ClientUser(std::function<void()> prepared);
	void prepare() override;
private:
	std::function<void()> m_prepare_notify;
};

#endif /*PLAYER_CLIENT_H*/
