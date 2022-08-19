#include <CL/sycl.hpp>
#include <iostream>
#include <random>
#include <vector>
// Header locations and some DPC++ extensions changed between beta09 and beta10
// Temporarily modify the code sample to accept either version
#define BETA09 20200827
#if __SYCL_COMPILER_VERSION <= BETA09
#include <CL/sycl/intel/fpga_extensions.hpp>
namespace INTintel;  // Namespace alias for backward compatibility
#else
#include <sycl/ext/intel/fpga_extensions.hpp>
#endif

using namespace sycl;

const size_t pipe_entries = 16;   // ensure 512 bit burst
using a_pipe = ext::intel::pipe<  // Defined in the SYCL headers. also ext::intel::pipe
    class a_read_pipe,            // An identifier for the pipe.
    float,                        // The type of data in the pipe.
    pipe_entries>;

using pipe_a = ext::intel::pipe<class id_a, int, 8>;
using pipe_b = ext::intel::pipe<class id_b, int, 8>;
using pipe_c = ext::intel::pipe<class id_c, int, 8>;

int problem_size;
// please complete
void reader_A(queue& q, buffer<float, 1>& input_buffer) {
  std::cout << "Enqueuing producer A...\n";
  auto e = q.submit([&](handler& h) {
    accessor input_accessor(input_buffer, h, read_only);
    auto num_elements = input_buffer.size();
    h.single_task([=]() {
      for (size_t i = 0; i < num_elements; ++i) {
        pipe_a::write(input_accessor[i]);
      }
    });
  });
}

void reader_B(queue& q, buffer<float, 1>& input_buffer) {
  std::cout << "Enqueuing producer B...\n";
  auto e = q.submit([&](handler& h) {
    accessor input_accessor(input_buffer, h, read_only);
    auto num_elements = input_buffer.size();
    h.single_task([=]() {
      for (size_t i = 0; i < num_elements; ++i) {
        pipe_b::write(input_accessor[i]);
      }
    });
  });
}

void adder(queue& q, int n) {
  // size_t num_elements = output_buffer.get_count();
  auto e = q.submit([&](handler& h) {
    h.single_task([=]() {
      // for (size_t i = 0; i < n; ++i) {
      int input_a = pipe_a::read();
      int input_b = pipe_b::read();

      // int answer = ConsumerWork(input);
      // output_accessor[i] = answer;
      pipe_c::write(input_a + input_b);
      // }
    });
  });
}

void writer_C(queue& q, buffer<float, 1>& output_buffer) {
  std::cout << "Enqueuing consumer C...\n";
  q.submit([&](handler& h) {
     accessor out_accessor(output_buffer, h, write_only, no_init);
     size_t num_elements = output_buffer.size();
     h.single_task([=]() {
       for (size_t i = 0; i < num_elements; ++i) {
         int input = pipe_c::read();
         out_accessor[i] = input;
       }
     });
   }).wait();
}

int main() {
  const size_t n = 32;
  problem_size
      std::vector<float>
          A, B, C(n);

  A.reserve(n);
  B.reserve(n);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  float value = dis(gen);
  A.push_back(value);
  B.push_back(value - 1.0f);

  for (size_t i = 1; i < n; ++i) {
    A.push_back(A[0] + i);
    B.push_back(B[0] + i);
  }

#if defined(FPGA_EMULATOR)
  ext::intel::fpga_emulator_selector device_selector;
#else
  ext::intel::fpga_selector device_selector;
#endif

  buffer buf_A(A);
  buffer buf_B(B);
  buffer buf_C(C);

  // sycl::default_selector device_selector;
  queue q(device_selector);

  reader_A(q, buf_A);
  reader_B(q, buf_B);
  adder(q);
  writer_C(q, buf_C);

  // property list to enable SYCL profiling for the device queue
  // auto props = property_list{property::queue::enable_profiling()};
  // auto producerFactory(std::vector<float> & vec, pipe & p) {

  for (int i = 0; i < 8; i++) {
    std::cout << "C[" << i << "] = " << C[i] << std::endl;
  }
}
