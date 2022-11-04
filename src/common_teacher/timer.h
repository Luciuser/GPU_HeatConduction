#pragma once

#include <time.h>
#include <omp.h>

struct Timer {
	double last, total, then;

	Timer() : last(0), total(0) {
		tick();
	}

	void tick() {
		then = omp_get_wtime();
	}
	
	void tock() {
		double now = omp_get_wtime();
		last = now - then;
		total += last;
		then = now;
	}

	double tock2() {

		double now = omp_get_wtime();
		return now - then;
	}

	void current(int &y, int &m, int &d)
	{
		time_t  t;
		tm  *tp;
		t = time(NULL);
		//tp = localtime(&t);
		localtime_s(tp, &t);

#if 0
		printf("%d/%d/%d/n", tp->tm_mon + 1, tp->tm_mday, tp->tm_year + 1900);
		printf("%d:%d:%d/n", tp->tm_hour, tp->tm_min, tp->tm_sec);
#endif

		m = tp->tm_mon + 1;
		d = tp->tm_mday;
		y = tp->tm_year + 1900;
	}
};
