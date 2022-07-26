#include <bits/stdc++.h>

#include <CL/sycl.hpp>
#include <add_array.hh>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace add_array;

int main() {
  std::vector<float> A, B, C;

  // ensure the vectors are contiguous
  A.reserve(n);
  B.reserve(n);
  C.reserve(n);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  // use a random value to initialize the first element to avoid compiler
  // optimizations
  float value = dis(gen);
  A.push_back(value);
  B.push_back(value - 1.0f);
  C.push_back(0);

  // initialize the array
  for (int i = 1; i < n; i++) {
    A.push_back(value);
    B.push_back(value - 1.0f);
    C.push_back(0);
  }

  auto start = std::chrono::high_resolution_clock::now();
  // add the two vectors
  {
    sycl::buffer bufA{A}, bufB{B}, bufC{C};
    sycl::queue q;
    q.submit([&](sycl::handler &h) {
      auto A = bufA.get_access(h, sycl::read_only);
      auto B = bufB.get_access(h, sycl::read_only);
      auto C = bufC.get_access(h, sycl::write_only);
      // Codigo para ejecutar paralelo
      h.parallel_for(n, [=](auto i) {
        C[i] = A[i] + B[i];
      });
    });
  }
  auto end = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 7; i++) {
    std::cout << C[i] << " ";
  }

  double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::endl
            << "Parallel time " << time_taken * 1e-9 << std::setprecision(10) << std::endl;
}
