#include <CL/sycl.hpp>
#include <iostream>
#include <random>
#include <vector>

namespace {
constexpr size_t n = 1024;
};

class optimized;

int main() {
  std::vector<float> A, res(1), mul_copies;

  A.reserve(n);
  mul_copies.reserve(n);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, 0.25f);

  float value = dis(gen);
  A.push_back(value);

  for (size_t i = 1; i < n; ++i) {
    A.push_back(A[0] + static_cast<float>(i) / static_cast<float>(n));
  }

  // please complete

  // queue q;
  // buffer buf(stepResults);
  // q.submit([&](handler &h) {
  //   accessor s(buf, h);
  //   h.single_task([=]()) {
  //   }
  // })

  float mul = 1.0f;
  // filleamos mul_copies con 1.0f
  for (size_t i = 0; i < n; ++i) {
    mul_copies.push_back(mul);
  }
  for (size_t i = 0; i < n; i++) {
    // multiplicamos el valor del ultimo * el actual
    auto cur = mul_copies[n - 1] * A[i];
#pragma unroll
    for (size_t j = n - 1; j > 0; j--) {
      // shiftea todo el resto a la derecha
      mul_copies[j] = mul_copies[j - 1];
    }
    // guardamos en el primer indice el actual
    mul_copies[0] = cur;
  }

#pragma unroll
  for (size_t i = 0; i < n; i++) {
    mul *= mul_copies[i];
  }

  std::cout
      << "res[0] = " << mul << std::endl;
}
