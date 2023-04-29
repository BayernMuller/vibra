#include "../utils/fft.h"
#include "../utils/calculation.h"
#include <iostream>
#include <iomanip>

template <typename Iterable>
void print(const char* name, const Iterable& input)
{
    std::cout << name << ": [";
    for (auto& i : input)
    {
        std::cout << i;
        if (&i != &input.back())
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

int main()
{
    FFT::RealArray input{1, 255, 1, 255, 1, 255, 1, 255};
    FFT::RealArray real;
    FFT::RealArray imag;
    FFT::RFFT(input, real, imag);

    print("input", input);
    print("real", real);
    print("imag", imag);

    calculate::square(real);
    calculate::square(imag);

    print("real^2", real);
    print("imag^2", imag);

    auto result = calculate::add(real, imag);
    print("real^2 + imag^2", result);

    result = calculate::devide(result, 1 << 17);
    print("(real^2 + imag^2) / (1 << 17)", result);

    calculate::max(result, 0.0000000001);
    print("max((real^2 + imag^2) / (1 << 17), 0.0000000001)", result);
    return 0;
}