#include <iomanip>
#include <iostream>
#include <limits>
#include <cmath>

using my_float = float;

auto pi_taylor(size_t steps) -> my_float {
    float piCalc = 0;
    for(int i = 0; i < steps; i++) {
        piCalc += 4*pow(-1,i)/(2*i +1);
    }

    return piCalc;
}

int main(int argc, const char *argv[]) {

    // read the number of steps from the command line
    if (argc != 2) {
        std::cerr << "Invalid syntax: pi_taylor <steps>" << std::endl;
        exit(1);

    }

    size_t steps = std::stoll(argv[1]);
    auto pi = pi_taylor(steps);

    std::cout << "For " << steps << ", pi value: "
        << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
        << pi << std::endl;
}
