#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using my_float = float;
using tid_time = std::pair<std::thread::id, double>;

template <typename F, typename... Args>
tid_time time_ms(F f, Args &&...args) {
  auto start = std::chrono::steady_clock::now();
  f(std::forward<Args>(args)...);
  auto stop = std::chrono::steady_clock::now();

  auto tid = std::this_thread::get_id();
  double ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
  return std::make_pair(tid, ex_time);
}

auto pi_taylor(size_t steps) -> my_float {
  float piCalc = 0;
  for (int i = 0; i < steps; i++) {
    piCalc += 4 * pow(-1, i) / (2 * i + 1);
  }

  std::cout << "For " << steps << ", pi value: "
            << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
            << piCalc << std::endl;
}

int main(int argc, const char *argv[]) {
  // read the number of steps from the command line
  if (argc != 2) {
    std::cerr << "Invalid syntax: pi_taylor <steps>" << std::endl;
    exit(1);
  }

  size_t steps = std::stoll(argv[1]);
  tid_time time = time_ms(pi_taylor, steps);
  std::cout << "Tiempo tardado " << time.second << std::endl;
}
