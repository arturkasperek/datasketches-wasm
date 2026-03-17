#include <cstdint>
#include <iostream>
#include <unordered_set>

#include "hll.hpp"

int main() {
  datasketches::hll_sketch sketch(12, datasketches::HLL_8);
  std::unordered_set<std::uint64_t> seen;

  for (std::uint64_t i = 0; i < 10000; ++i) {
    sketch.update(i);
    seen.insert(i);
  }

  for (std::uint64_t i = 0; i < 2500; ++i) {
    sketch.update(i);
  }

  std::cout << "Exact unique count: " << seen.size() << '\n';
  std::cout << "HLL estimate: " << sketch.get_estimate() << '\n';
  std::cout << "95% confidence interval: ["
            << sketch.get_lower_bound(2) << ", "
            << sketch.get_upper_bound(2) << "]\n";

  return 0;
}
