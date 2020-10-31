#pragma once

#include <algorithm>
using namespace std;

template <typename It>
class IteratorRange {
public:
  IteratorRange(It first, It last) : first(first), last(last) {
  }

  It begin() const {
    return first;
  }

  It end() const {
    return last;
  }

  size_t size() const {
    return last - first;
  }

private:
  It first, last;
};

template <typename Container>
auto Head(Container& c, int top) {
  return IteratorRange(begin(c), begin(c) + min<size_t>(max(top, 0), c.size()));
}

template <typename It>
class FIteratorRange {
public:
  FIteratorRange(It first, It last) : first(first), last(last) {
  }

  It begin() const {
    return first;
  }

  It end() const {
    return last;
  }

  size_t size() const {
    size_t n;
    for (auto it = first; it != last; it++) {
      ++n;
    }
    return n;
  }

private:
  It first, last;
};

template <typename Container>
auto FHead(Container& c, int top) {
  return FIteratorRange(begin(c), next(begin(c), min<size_t>(max(top, 0), c.size())));
}
