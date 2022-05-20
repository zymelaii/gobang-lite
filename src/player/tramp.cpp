#include "tramp.h"

#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <windows.h>
#include <future>
#include <thread>

void Tramp::prepare() {
	srand(time(0L));

	constexpr size_t N = gobang::BoardStatus::GridNumber;
	int row = -1, col = -1;
	do {
		row = rand() % N;
		col = rand() % N;
	} while (query(row, col) != gobang::ChessType::nil);

	assert(row != -1 && col != -1);

	std::packaged_task<void()> task([this, row, col]() {
		Sleep(rand() % 3000 + 600);
		this->drop(row, col);
	});
    std::thread(std::move(task)).detach();
}
