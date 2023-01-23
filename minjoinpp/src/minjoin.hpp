
#ifndef MINSIMJOIN_MINJOIN_HPP
#define MINSIMJOIN_MINJOIN_HPP
#include <future>
#include <algorithm>
#include <set>
#include <cstring>
#include <thread>
#include <queue>
#include <unordered_map>
#include "utils.hpp"
#include "hash.hpp"

using std::vector;
using std::string;

template<class T, class Cmp>
vector<T> merge(const vector<vector<T>> &arr, Cmp cmp) {
  using S = std::pair<T, size_t>;
  auto f = [&cmp](const S &a, const S &b) {
    if (cmp(a.first, b.first) == cmp(b.first, a.first)) {
      return a.second < b.second;
    } else {
      return cmp(b.first, a.first);
    }
  };
  std::priority_queue<S, std::vector<S>, std::function<bool(const S &, const S &)>> queue(f);
  size_t total = 0;
  vector<size_t> cnt(arr.size(), 0);
  for (size_t i = 0; i < arr.size(); ++i) {
    total += arr[i].size();
    fprintf(stderr, "%d ", (int) arr[i].size());
    if (!arr[i].empty()) {
      queue.push(S(arr[i].front(), i));
    }
  }
  fprintf(stderr, "\n");
  vector<T> result;
  result.reserve(total);
  while (!queue.empty()) {
    auto cur = queue.top();
    queue.pop();
    result.push_back(cur.first);
    if (cnt[cur.second] + 1 < arr[cur.second].size()) {
      cnt[cur.second]++;
      queue.push(S(arr[cur.second][cnt[cur.second]], cur.second));
    }
  }
  for (int i = 0; i < (int) arr.size(); i++) fprintf(stderr, "%lu%c", cnt[i], " \n"[i + 1 == (int) arr.size()]);
  fprintf(stderr, "->%d (%zu)\n", (int) result.size(), total);
  return result;
}
vector<size_t> generate_anchors(const vector<uint64_t> &q_hash, int W, int T) {
  vector<size_t> output;
  output.push_back(0);
  int len = (int) q_hash.size() + W - 1;
  int L = (int) len / T;
  int z = std::max(1, int(0.7 * L / 2));
  for (int i = z; i + z + W - 1 < len;) {
    int d = 1;
    for (; d <= z && q_hash[i] < q_hash[i - d] && q_hash[i] < q_hash[i + d]; ++d);
    if (d == z + 1)
      output.push_back(i);
    i += d;
  }
  output.push_back(len);
  return output;
}

signature_t create_signature(const string &s, const vector<size_t> &anchors, int T) {
  signature_t output;
  auto x = s.data();
  int bound = (int) ((double) s.size() * 0.5 / T);
  for (auto i = 1u; i < anchors.size(); ++i) {
    if ((int) (anchors[i] - anchors[i - 1]) < bound) continue;
    auto val = seahash(x + anchors[i - 1], anchors[i] - anchors[i - 1]);

    output.push_back(part_t{val, anchors[i - 1],
                            s.length() - anchors[i]});
  }
  return output;
}

void partition(vector<record_t> &records, int W, int T, size_t base, int nthreads) {
  size_t max_len = 0;
  for (auto &x: records) {
    max_len = std::max(max_len, x.s.size());
  }
  auto power = std::vector<size_t>(max_len + 1, 1);
  auto f = [&](size_t shift) {
    for (auto i = shift; i < records.size(); i += nthreads) {
      auto &record = records[i];
      auto s = record.s.data();
      vector<uint64_t> q(record.s.length() - W + 1);
      for (int j = 0; j < (int) record.s.length() - W + 1; j++) {
        q[j] = murmur3_32(reinterpret_cast<const uint8_t *>(s + j), W, base);
      }

      auto anchors = generate_anchors(q, W, T);

      record.sign = create_signature(record.s, anchors, T);
    }
  };
  vector<std::future<void>> tasks(nthreads);
  for (auto i = 0; i < nthreads; ++i) {
    tasks[i] = std::async(std::launch::async, f, i);
  }
  for (auto i = 0; i < nthreads; ++i) {
    tasks[i].get();
  }
}

void join(const vector<record_t> &records,
          const size_t K,
          vector<candidate_t> &candidates,
          double threshold, int nthreads) {
  struct rec_t {
    size_t id;
    size_t sid;
  };
  if (nthreads >= 1) {
    fprintf(stderr, "threshold = %lf\n", threshold);
    vector<rec_t> a;
    {
      auto cmp = [&records](auto &o1, auto &o2) {
        if (records[o1.id].sign[o1.sid].hash != records[o2.id].sign[o2.sid].hash) {
          return records[o1.id].sign[o1.sid].hash < records[o2.id].sign[o2.sid].hash;
        }
        return records[o1.id].sign[o1.sid].plen < records[o2.id].sign[o2.sid].plen;
      };
      vector<std::future<vector<rec_t>>> tasks(nthreads);
      auto f = [&](size_t shift) {
        vector<rec_t> res;
        for (size_t i = shift; i < records.size(); i += nthreads) {
          for (size_t j = 0; j < records[i].sign.size(); ++j) {
            res.push_back(rec_t{i, j});
          }
        }
        std::sort(begin(res), end(res), cmp);
        return res;
      };
      for (int i = 0; i < nthreads; i++) tasks[i] = std::async(std::launch::async, f, i);
      vector<vector<rec_t>> arr(nthreads);
      for (int i = 0; i < nthreads; i++) arr[i] = tasks[i].get();
      a = merge(arr, cmp);
      /*for (int i = 0; i + 1 < (int) a.size(); i++)
        assert(records[a[i].id].sign[a[i].sid].hash <= records[a[i + 1].id].sign[a[i + 1].sid].hash);*/
    }

    vector<std::pair<size_t, size_t>> intervals;
    for (size_t i = 0; i < a.size(); ++i) {
      auto j = i;
      while (j < a.size() && records[a[i].id].sign[a[i].sid].hash == records[a[j].id].sign[a[j].sid].hash) {
        j++;
      }
      if (j - i > 1) {
        intervals.emplace_back(i, j);
      }
      i = j;
    }
    fprintf(stderr, "intervals size = %lu\n", intervals.size());
    vector<std::future<vector<std::pair<int, int>>>> tasks(nthreads);
    auto f = [&](size_t shift) {
      vector<std::pair<int, int>> output;
      for (auto index = shift; index < intervals.size(); index += nthreads) {
        for (auto i = intervals[index].first; i < intervals[index].second; ++i) {
          std::set < size_t > v;
          for (auto j = i + 1; j < intervals[index].second; ++j) {
            if (a[i].id == a[j].id
                || abs((int) records[a[i].id].s.length() - (int) records[a[j].id].s.length()) > (int) K) {
              continue;
            }
            const auto &o1 = records[a[i].id].sign[a[i].sid];
            const auto &o2 = records[a[j].id].sign[a[j].sid];
            if (o2.plen - o1.plen > K) {
              break;
            }
            auto diff = std::abs((int) o2.plen - (int) o1.plen);
            diff += std::abs((int) o2.slen - (int) o1.slen);
            if (diff > (int) K) {
              continue;
            }

            auto ind1 = a[i].id;
            auto ind2 = a[j].id;
            if (v.find(ind2) != v.end()) {
              continue;
            }
            v.insert(ind2);
            if (ind1 > ind2)
              std::swap(ind1, ind2);
            output.emplace_back(ind1, ind2);
          }
        }
      }
      sort(begin(output), end(output));
      return output;
    };
    for (int i = 0; i < nthreads; ++i) {
      tasks[i] = std::async(std::launch::async, f, i);
    }

    vector<vector<std::pair<int, int>>> res(nthreads);
    for (int i = 0; i < nthreads; ++i) {
      res[i] = tasks[i].get();
    }
    std::vector<std::pair<int, int>> output = merge(res, std::less<>());
//  for (int i = 0; i + 1 < (int) output.size(); i++) assert(output[i] <= output[i + 1]);
    for (size_t i = 0; i < output.size();) {
      auto j = i;
      while (j < output.size() && output[i] == output[j]) {
        j++;
      }
      if ((double) (j - i) >= threshold) {
        candidates.emplace_back(output[i]);
      }
      i = j;
    }
  } else {
    fprintf(stderr, "fast...\nthreshold = %lf\n", threshold);
    std::unordered_map<size_t, vector<rec_t>> table;
    vector<std::pair<int, int>> output;
    for (size_t i = 0; i < records.size(); i++) {
      for (size_t j = 0; j < records[i].sign.size(); ++j) {
        std::set < size_t > v;
        if (!table.count(records[i].sign[j].hash))
          continue;
        for (auto &x: table[records[i].sign[i].hash]) {
          const auto &o1 = records[i].sign[j];
          const auto &o2 = records[x.id].sign[x.sid];
          auto diff = std::abs((int) o2.plen - (int) o1.plen);
          diff += std::abs((int) o2.slen - (int) o1.slen);
          if (diff > (int) K) {
            continue;
          }

          auto ind1 = i;
          auto ind2 = x.id;
          if (v.find(ind2) != v.end()) {
            continue;
          }
          v.insert(ind2);
          if (ind1 > ind2)
            std::swap(ind1, ind2);
          output.emplace_back(ind1, ind2);
        }
      }
      for (size_t j = 0; j < records[i].sign.size(); ++j) {
        table[records[i].sign[j].hash].emplace_back(rec_t{i, j});
      }
    }

    fprintf(stderr, "size of a = %d\n", (int) output.size());
    sort(begin(output), end(output));
    for (size_t i = 0; i < output.size();) {
      auto j = i;
      while (j < output.size() && output[i] == output[j]) {
        j++;
      }
      if ((double) (j - i) >= threshold) {
        candidates.emplace_back(output[i]);
      }
      i = j;
    }
  }
}

void verification(const vector<candidate_t> &candidates,
                  const vector<record_t> &records,
                  const int K, vector<candidate_t> &result, int nthreads) {
  auto foo = [&](size_t shift) {
    vector<candidate_t> output;
    for (auto i = shift; i < candidates.size(); i += nthreads) {
      auto first = candidates[i].first;
      auto second = candidates[i].second;
      auto distance = edit(records[first].s.cbegin(),
                           records[first].s.cend(),
                           records[second].s.cbegin(),
                           records[second].s.cend(),
                           K);
      if (0 <= distance && distance <= K) {
        output.push_back(candidates[i]);
      }
    }
    return output;
  };
  vector<std::future<vector<candidate_t>>> tasks(nthreads);
  for (auto i = 0; i < nthreads; ++i) {
    tasks[i] = std::async(std::launch::async, foo, i);
  }
  for (auto i = 0; i < nthreads; ++i) {
    auto output = tasks[i].get();
    for (const auto &x: output) {
      result.push_back(x);
    }
  }
}

#endif //MINSIMJOIN_MINJOIN_HPP
