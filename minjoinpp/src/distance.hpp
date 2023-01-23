#ifndef MINSIMJOIN_DISTANCE_HPP
#define MINSIMJOIN_DISTANCE_HPP

#include <vector>

template<class T>
int slide(T first1, T last1, T first2, T last2) {
  auto it1 = first1;
  auto it2 = first2;
  while (true) {
    if (std::distance(it1, last1) >= 8 && std::distance(it2, last2) >= 8) {
      if (*((long long *) &(*it1)) == *((long long *) &(*it2))) { //This line leads to UB for so many fucking reasons
        it1 += 8;
        it2 += 8;
      } else {
        auto val = (__builtin_ctzll(*((long long *) &(*it1)) - *((long long *) &(*it2))) >> 3);
        return std::distance(first1, it1) + val;
      }
    } else {
      for (; it1 != last1 || it2 != last2; ++it1, (void) ++it2)
        if (*it1 != *it2)
          break;
      return std::distance(first1, it1);
    }
  }
}

template<class T>
int edit(T first1, T last1, T first2, T last2, int k) {
  auto s_length = (int) (std::distance(first1, last1));
  auto t_length = (int) (std::distance(first2, last2));
  if (s_length > t_length) {
    return edit(first2, last2, first1, last1, k);
  }
  if (abs(s_length - t_length) > k) {
    return k + 1;
  }
  assert(s_length <= t_length);
  auto diff = t_length - s_length;
  assert(diff >= 0);
  int shift = k + 1;
  auto mema = std::vector<int>(2 * k + 3, -1);
  auto memb = std::vector<int>(2 * k + 3, -1);
  auto &a = mema;
  auto &b = memb;
  for (int h = 0; h <= k; h++) {
    swap(a, b);
    auto p = shift - std::min(1 + (k - diff) / 2, h);
    auto q = shift + std::min(1 + k / 2 + diff, h);
    for (int i = p; i <= q; i++) {
      int r = std::max(std::max(a[i - 1], a[i] + 1), a[i + 1] + 1);
      if (r >= s_length || (r + i - shift) >= t_length) {
        b[i] = r;
      } else {
        b[i] = slide(first1 + r, last1, first2 + (r + i - shift), last2) + r;
      }
      if (((i + s_length) == (t_length + shift)) && b[i] >= s_length) {
        return h;
      }
    }
  }
  return k + 1;
}

#endif //MINSIMJOIN_DISTANCE_HPP
