#include <iostream>
#include <string>
#include <cassert>
#include <cstdio>
#include "distance.hpp"
#include "utils.hpp"
#include "minjoin.hpp"
#include "hash.hpp"
#include <random>

std::random_device rd;

std::vector<candidate_t> algorithm(vector<record_t> &records,
                                   int K,
                                   int T,
                                   int W,
                                   double S,
                                   int nthreads) {
  auto init_time = current_time();
  int total = 0;
  fprintf(stderr, "total = %d\n", total);
  auto read_time = current_time();
  auto readtime = get_time(init_time, read_time);
  fprintf(stderr, "read time = %lfs\n", get_time(init_time, read_time));

  auto base = rd();
  fprintf(stderr, "base = %d\n", base);
  partition(records, W, T, base, nthreads);
  auto part_time = current_time();
  fprintf(stderr, "partition time = %lfs\n", get_time(read_time, part_time));
  auto parttime = get_time(read_time, part_time);
  vector<candidate_t> candidates;
  join(records, K, candidates, S, nthreads);
  auto join_time = current_time();
  fprintf(stderr, "join time (%lu) = %lfs\n", candidates.size(), get_time(part_time, join_time));
  auto jointime = get_time(part_time, join_time);
  vector<candidate_t> result;
  verification(candidates, records, K, result, nthreads);
  auto verify_time = current_time();
  fprintf(stderr, "verify time(%lu) = %lfs\n", result.size(), get_time(join_time, verify_time));
  auto verifytime = get_time(join_time, verify_time);
  auto total_time = get_time(init_time, verify_time);
  printf("%s,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%d\n",
         "MinJoin++",
         K,
         T,
         W,
         (int) S,
         nthreads,
         readtime,
         parttime,
         jointime,
         verifytime,
         total_time,
         (int) result.size());
  fflush(stdout);
  return result;
}

// MinJoin++,250,50,10,1,1,1.179817,1.709188,0.578047,1.776106,5.243158,4585

int main(int argc, char **argv) {
//  freopen("output.txt", "wt", stdout);
  std::string filename;
//  assert(argc >= 7);
  filename = argv[1];
  int K = atoi(argv[2]);
//  int T = int(atof(argv[3]));
  int W = atoi(argv[3]);
//  double S = atof(argv[5]);
//  int nthreads = atoi(argv[6]);
//  int repeats = atoi(argv[7]);
  vector<record_t> records;
  read(filename, records);
  vector<record_t> sample;
  std::uniform_int_distribution<int> distribution(0, 9);
  for (auto &x: records) {
    if (distribution(rd) == 0) {
      sample.push_back(x);
    }
  }
  vector<candidate_t> total;
  double T = 10;
  vector<candidate_t> res0;
  while (1) {
    int ok = 1;
    for (int i = 0; i < 10 && ok; i++) {
      auto res1 = algorithm(sample, K, (int)T, W, T / 20, 1);
      fprintf(stderr, "res = (%lu %lu)\n", res0.size(), res1.size());
      ok &= res0 == res1;
      if (res1.size() > res0.size()) {
        res0 = res1;
      }
    }
    if (ok) {
      break;
    }
    T = 2 * T;
    fprintf(stderr, "T = %f\n", T);
  }
  auto r = algorithm(records, K, (int)T, W, T / 20, 1);
  /*for (int iter = 0; iter < repeats; ++iter) {
    auto r = algorithm(filename, K, T, W, S, 1, 0);
    for (auto x: r) total.push_back(x);
    sort(begin(total), end(total));
    total.resize(unique(begin(total), end(total)) - begin(total));
    fprintf(stderr, "total = %d\n", (int) total.size());
  }*/
  /*printf("%s,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%d\n",
         "MinJoinT",
         K,
         T,
         W,
         (int) S,
         nthreads,
         0.0,
         0.0,
         0.0,
         0.0,
         0.0,
         (int) total.size());*/
}
