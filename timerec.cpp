#include "timerec.h"

#ifdef _WIN32 
void recordtime(clock_t& t)
{
	t = clock();
}
void updatetime(float& result, clock_t& a, clock_t& b)
{
	result += ((float)b - (float)a) / CLOCKS_PER_SEC;
}
#else
void recordtime(struct timeval& t)
{
	gettimeofday(&t, NULL);
}
void updatetime(float& result, struct timeval& a, struct timeval& b)
{
	result += b.tv_sec + b.tv_usec / 1000000.0 - a.tv_sec - a.tv_usec / 1000000.0;
}
#endif