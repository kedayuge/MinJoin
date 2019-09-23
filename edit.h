
#include <vector>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <numeric>
#include <math.h>
using namespace std;



 

int edit_dp(const string& s1, const string& s2);

int edit_dp(const string& s1, const string& s2, const int k);

#ifdef __linux__

#define K  (20000)
typedef int64_t int64;
typedef int32_t int32;

int slide(const char *x, const char *y);

int slide32(const char *x, const char *y);

int edit_dp(const char *x, const int x_len, const  char *y, const int y_len, int k);

#endif
