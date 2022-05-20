#include "client.h"

ClientUser::ClientUser(std::function<void()> prepared)
	: m_prepare_notify(prepared) {
}

void ClientUser::prepare() {
	m_prepare_notify();
}
