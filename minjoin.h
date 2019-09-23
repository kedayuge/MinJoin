#pragma once
#include <vector> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <algorithm> 
#include <tuple>
#include <omp.h>
#include <numeric> 
#include <math.h>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <map>
#include <climits>
#include "hash.h"
#include "edit.h"
#include "timerec.h"
#include "partition.h"
using namespace std;

extern int k_input;
extern string filename;

extern vector<string> oridata; //original strings
extern vector<int> indices;
extern vector<string> oridata_modified; //modified strings for edit distance computation

extern int seg;
extern int W;
extern bool outputresult;

extern unordered_map<string, list<pair<int, int>>> hash_table;
extern vector<pair<int, int>> candidate;
extern vector<pair<int, int>> outputs;

extern double chunkrate;



#ifdef _WIN32 
extern clock_t tstart, tend, tstart1, tend1;
#else
extern struct timeval tstart, tend, tstart1, tend1;
#endif
extern float partitime; // time for partion strings

void readdata();

vector<pair<string, int>> partition(string& s);

void join();

void verify();

int main(int argc, char **argv);
