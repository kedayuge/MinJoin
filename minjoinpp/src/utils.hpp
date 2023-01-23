#ifndef MINSIMJOIN_UTILS_HPP
#define MINSIMJOIN_UTILS_HPP

#include <sys/time.h>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>

struct part_t {
  uint64_t hash;
  size_t plen;
  size_t slen;
};
using signature_t = std::vector<part_t>;
using candidate_t = std::pair<size_t, size_t>;

struct record_t {
  std::string s;
  signature_t sign;
};

struct timeval current_time() {
  struct timeval res{};
  gettimeofday(&res, nullptr);
  return res;
}

double get_time(const struct timeval &a, const struct timeval &b) {
  return (double) (b.tv_sec - a.tv_sec) + (b.tv_usec - a.tv_usec) / 1e6;
}

void read(const std::string &filename, std::vector<record_t> &records) {
  std::ifstream file(filename);
  std::string str;
  std::vector<std::string> data;
  while (getline(file, str)) {
    data.push_back(str);
  }
  records.resize(data.size());
  size_t max_len = 0;
  for (auto i = 0u; i < records.size(); i++) {
    records[i].s = data[i];
    max_len = std::max(max_len, data[i].length());
  }
//  fprintf(stderr, "max len = %zu\n", max_len);
}

#endif //MINSIMJOIN_UTILS_HPP
