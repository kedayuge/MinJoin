#include <vector> 
#include <iostream>
#include <string.h>
#include <algorithm> 
#include <numeric> 
#include <math.h>
#include <unordered_map>

using namespace std;

extern int hashbit;
extern int hashmask;

extern int dictsize;
extern unordered_map<char, int> dict;

void builddict();

void initiHash();

vector<size_t> hashstring(string &s);