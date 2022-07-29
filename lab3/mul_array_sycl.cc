#include <CL/sycl.hpp>
#include <iostream>
#include <random>
#include <vector>

namespace {
constexpr size_t n = 1024;
};

class naive;

int main() {
  std::vector<float> A, res(1);

  A.reserve(n);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, 0.25f);

  float value = dis(gen);
  A.push_back(value);

  for (size_t i = 1; i < n; ++i) {
    A.push_back(A[0] + static_cast<float>(i) / static_cast<float>(n));
  }

  // please complete
  // aca faltaria mandar a la fpga
  // q.submit(blah handler)
  // creas accessors,
  // h.single_task([=]()){
  // codigo que tenemos abajo
  // }

  sycl::queue q;
  sycl::buffer buf_A(A);
  sycl::buffer buf_res(res);
  q.submit([&](sycl::handler &h) {
    // sycl::accessor s(buf_A, h);
    // sycl::accessor s(buf_res, h);
    auto accessor_read = buf_A.get_access(h, sycl::read_only);
    auto accessor_write = buf_res.get_access(h, sycl::write_only);
    res[0] = 1.0f;
    auto device_task = [=]() {
      for (size_t i = 0; i < A.size(); i++) {
        // res[0] *= A[i];
        accessor_write[0] *= accessor_read[i];
      }
    };
    h.single_task(device_task);
    // [=]() {
    // for (size_t i = 0; i < A.size(); i++) {
    //   res[0] *= A[i];
    // }
  });
  // });

  std::cout
      << "res[0] = " << res[0] << std::endl;
}
