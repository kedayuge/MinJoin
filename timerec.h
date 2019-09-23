#ifdef __unix__                    
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#endif
#include <time.h>


#ifdef _WIN32 
void recordtime(clock_t& t);

void updatetime(float& result, clock_t& a, clock_t& b);

#else
void recordtime(struct timeval& t);

void updatetime(float& result, struct timeval& a, struct timeval& b);
#endif
