#ifndef PLAYER_TRAMP_H
#define PLAYER_TRAMP_H

#include "../gobang/gobang.h"

class Tramp : public gobang::Player {
public:
	void prepare() override;
};

#endif /*PLAYER_TRAMP_H*/
