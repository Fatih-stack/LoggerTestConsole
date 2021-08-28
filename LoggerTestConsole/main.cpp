#pragma once

#include "Logger.h"
#include <assert.h>
#include <thread>

using namespace aricanli::general;

void log_test(int n) {
	WLOG_VERB(L"Verb %d", n, L"abcd test");
}

int main()
{
	Timer t;
	for (int i = 0; i < 100'000; i++)
		log_test(i);

	return 0;
}