#pragma once

#include "Logger.h"
#include <assert.h>
#include <thread>

using namespace aricanli::general;

void log_test(int n) {
	LOG_DEBUG(L"fatih", n, L"Write Args", 3434);
	LOG_WARN(L"warning %d", n, "sdsdsd", L"fatih", 7853);
	LOG_ERROR("error %d", n, "Args Errrrrrrorrrrr");
	LOG_FATAL("fatal error %d", n);
	LOG_TRACE("Trace : %d", n);
	LOG_QUIET("QUIET %d", n);
}

int main()
{
	std::thread threads[3];
	for (int i = 0; i < 3; i++)
		threads[i] = std::thread(log_test, i);

	for (int i = 0; i < 3; i++)
		threads[i].join();

	return 0;
}