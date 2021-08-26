#pragma once

#include "Logger.h"
#include <assert.h>
#include <thread>

using namespace aricanli::general;

void log_test(int n) {
	WLOG_DEBUG(L"fatih %d", n, L"Write Args", 3434);
	WLOG_WARN(L"warn %d", n, L"Warningggggg");
	SLOG_ERROR("error %d", n, "Error writing");
	LOG_FATAL(L"fatal %d", n, "Fatal reading");
	WLOG_TRACE(L"Trace %d", n, L"trace writing");
	WLOG_QUIET(L"QUIET %d", n);
}

int main()
{
	Timer t;
	std::thread threads[3];
	for (int i = 0; i < 3; i++)
		threads[i] = std::thread(log_test, i);

	for (int i = 0; i < 3; i++)
		threads[i].join();

	return 0;
}