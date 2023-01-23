#include <iostream>
#include <string>
#include <cassert>
#include <cstdio>
#include "distance.hpp"
#include "utils.hpp"
#include "minjoin.hpp"
#include <random>

std::random_device rd;

std::vector<candidate_t> algorithm(const std::string &filename,
                                   int K,
                                   int T,
                                   int W,
                                   double S,
                                   int nthreads) {
  std::vector<record_t> records;
  auto init_time = current_time();
  read(filename, records);
  int total = 0;
  fprintf(stderr, "total = %d\n", total);
  auto read_time = current_time();
  auto readtime = get_time(init_time, read_time);
  fprintf(stderr, "read time = %lfs\n", get_time(init_time, read_time));

  auto base = rd();
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
  fprintf(stderr, "average verfication = %lfms\n", verifytime/(double)candidates.size() * 1e6);
  fflush(stdout);
  return result;
}

int main(int argc, char **argv) {
//  freopen("output.txt", "wt", stdout);
  std::string filename;
  assert(argc >= 7);
  filename = argv[1];
  int K = atoi(argv[2]);
  int T = int(atof(argv[3]));
  int W = atoi(argv[4]);
  double S = atof(argv[5]);
  int nthreads = atoi(argv[6]);
  int repeats = atoi(argv[7]);
  srand(time(nullptr));
  assert(nthreads >= 1);
  vector<candidate_t> total;
  for (int iter = 0; iter < repeats; ++iter) {
    auto r = algorithm(filename, K, T, W, S, nthreads);
    for (auto x: r) total.push_back(x);
    sort(begin(total), end(total));
    total.resize(unique(begin(total), end(total)) - begin(total));
    fprintf(stderr, "total = %d\n", (int) total.size());
  }
  printf("%s,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%d\n",
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
         (int) total.size());
}

// MinJoin++,250,50,10,1,1,1.179817,1.709188,0.578047,1.776106,5.243158,4585